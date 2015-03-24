/*
 * uart.c - functions to work with UART
 *
 * Copyright 2014 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "main.h"
#include "uart.h"
#include "cdcacm.h"

// Buffers for Tx
static UART_buff TX_buffer[3]; // Tx buffers for all three ports
static UART_buff RX_buffer[3]; // Rx buffers for all three ports

void fill_uart_RXbuff(uint32_t UART, uint8_t byte);

/**
 * Set UART speed
 * @param lc - UART parameters or NULL for value from cdcacm.c (started - B115200,8,N,1)
 */
void UART_setspeed(uint32_t UART, struct usb_cdc_line_coding *lc){
	uint32_t tmp;
	if(!lc) lc = &linecoding; // default linecoding from cdcacm.c
	if(!(lc->dwDTERate)) lc->dwDTERate = 115200;
	if(!(lc->bDataBits)) lc->bDataBits = 8;
	usart_set_baudrate(UART, lc->dwDTERate);
	usart_set_databits(UART, lc->bDataBits);
	switch(lc->bCharFormat){
		case USB_CDC_1_5_STOP_BITS:
			tmp = USART_STOPBITS_1_5;
		break;
		case USB_CDC_2_STOP_BITS:
			tmp = USART_STOPBITS_2;
		break;
		case USB_CDC_1_STOP_BITS:
		default:
			tmp = USART_STOPBITS_1;
	}
	usart_set_stopbits(UART, tmp);
	switch(lc->bParityType){
		case USB_CDC_ODD_PARITY:
			tmp = USART_PARITY_ODD;
		break;
		case USB_CDC_EVEN_PARITY:
			tmp = USART_PARITY_EVEN;
		break;
		case USB_CDC_NO_PARITY:
		default:
			tmp = USART_PARITY_NONE;
	}
	usart_set_parity(UART, tmp);
	usart_set_flow_control(UART, USART_FLOWCONTROL_NONE);
	usart_set_mode(UART, USART_MODE_TX_RX);
}

/**
 * Setup UART
 */
void UART_init(uint32_t UART){
	uint32_t irq, rcc, rccgpio, gpioport, gpiopin;
	switch(UART){
		case USART2: // 1-wire
			irq = NVIC_USART2_IRQ; // interrupt for given USART
			rcc = RCC_USART2;      // RCC timing of USART
			rccgpio = RCC_GPIOA;   // RCC timing of GPIO pin (for output)
			TX_buffer[1].end = 0;  // reset counters
			TX_buffer[1].start = 0;
			// output pin setup
			gpioport = GPIO_BANK_USART2_TX;
			gpiopin  = GPIO_USART2_TX;
		break;
		case USART3: // without remapping
			irq = NVIC_USART3_IRQ;
			rcc = RCC_USART3;
			rccgpio = RCC_GPIOB;
			TX_buffer[2].end = 0;
			TX_buffer[2].start = 0;
			gpioport = GPIO_BANK_USART3_TX;
			gpiopin  = GPIO_USART3_TX;
		break;
		case USART1:
		default:
			irq = NVIC_USART1_IRQ;
			rcc = RCC_USART1;
			rccgpio = RCC_GPIOA;
			TX_buffer[0].end = 0;
			TX_buffer[0].start = 0;
			gpioport = GPIO_BANK_USART1_TX;
			gpiopin  = GPIO_USART1_TX;
	}
	// enable clocking
	rcc_periph_clock_enable(RCC_AFIO); // alternate functions
	rcc_periph_clock_enable(rcc);      // USART
	rcc_periph_clock_enable(rccgpio);  // GPIO pins
	// enable output pin
	if(UART == OW_USART_X){ // one wire
		// TX: open-drain output
		gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_50_MHZ,
					GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, gpiopin);
		// RX: floating input
		gpio_set_mode(OW_RX_PORT, GPIO_MODE_INPUT,
					GPIO_CNF_INPUT_FLOAT, OW_RX_PIN);
	}else{
		gpio_set_mode(gpioport, GPIO_MODE_OUTPUT_50_MHZ,
					GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, gpiopin);
	}
	// enable IRQ
	nvic_enable_irq(irq);
	UART_setspeed(UART, NULL);
	// Enable UART receive interrupt
	USART_CR1(UART) |= USART_CR1_RXNEIE;
	// Enable UART
	usart_enable(UART);
}

/*
 * UART interrupts
 */
// common
void UART_isr(uint32_t UART){
	uint8_t bufidx = 0, data;
	UART_buff *curbuff;
	// Check if we were called because of RXNE
	if(USART_SR(UART) & USART_SR_RXNE){
		// parce incoming byte
		data = usart_recv(UART);
		fill_uart_RXbuff(UART, data);
	}
	// Check if we were called because of TXE -> send next byte in buffer
	if((USART_CR1(UART) & USART_CR1_TXEIE) && (USART_SR(UART) & USART_SR_TXE)){
		switch(UART){
			case USART1:
			bufidx = 0;
			break;
			case USART2:
				bufidx = 1;
			break;
			case USART3:
				bufidx = 2;
			break;
			default: // error - return
				return;
		}
		curbuff = &TX_buffer[bufidx];
		bufidx = curbuff->start; // start of data in buffer
		if(bufidx != curbuff->end){ // there's data in buffer
			// Put data into the transmit register
			usart_send(UART, curbuff->buf[bufidx]);
			if(++(curbuff->start) == UART_TX_DATA_SIZE) // bufidx > endidx && got end of buffer
				curbuff->start = 0;
		}else{ // Disable the TXE interrupt, it's no longer needed
			USART_CR1(UART) &= ~USART_CR1_TXEIE;
			// empty indexes
			curbuff->start = 0;
			curbuff->end = 0;
		}
	}
}
// particular interrupt handlers
void usart1_isr(){
	UART_isr(USART1);
}
void usart2_isr(){
	UART_isr(USART2);
}
void usart3_isr(){
	UART_isr(USART3);
}

// put byte into Tx buffer
void fill_uart_buff(uint32_t UART, uint8_t byte){
	UART_buff *curbuff;
	uint8_t bufidx = 0, endidx;
	if(!(USART_CR1(UART) & USART_CR1_UE)) return; // UART disabled
	USART_CR1(UART) &= ~USART_CR1_TXEIE; // disable TX interrupt while buffer filling
	while ((USART_SR(UART) & USART_SR_TXE) == 0); // wait until last byte send
	switch(UART){
		case USART1:
			bufidx = 0;
		break;
		case USART2:
			bufidx = 1;
		break;
		case USART3:
			bufidx = 2;
		break;
		default: // error - return
			return;
	}
	curbuff = &TX_buffer[bufidx];
	bufidx = curbuff->start; // start of data in buffer
	endidx = curbuff->end; // end of data
	curbuff->buf[endidx++] = byte; // put byte into buffer
	// now check indexes
	if(endidx != bufidx && endidx != UART_TX_DATA_SIZE){ // all OK - there's enough place for data
		(curbuff->end)++; // just increment index in buffer
	}else{ // dangerous situation: possible overflow
		if(endidx == UART_TX_DATA_SIZE){ // end of buffer
			if(bufidx != 0){ // no overflow
				curbuff->end = 0;
				goto end_of_fn;
			}
		}
		// overflow: purge all data
		bufidx = curbuff->start; // refresh data index
		for(endidx = bufidx; endidx < UART_TX_DATA_SIZE; endidx++) // first data porion
			usart_send(UART, curbuff->buf[endidx]);
		for(endidx = 0; endidx < bufidx; endidx++) // rest of data
			usart_send(UART, curbuff->buf[endidx]);
		curbuff->start = 0;
		curbuff->end = 0;
		return;
	}
	end_of_fn:
	// enable interrupts to send data from buffer
	USART_CR1(UART) |= USART_CR1_TXEIE;
}

/**
 * send data over UART - one function for each uart
 * @param byte - one byte to put in UART queue
 */
void uart1_send(uint8_t byte){
	fill_uart_buff(USART1, byte);
}
void uart2_send(uint8_t byte){
	fill_uart_buff(USART2, byte);
}
void uart3_send(uint8_t byte){
	fill_uart_buff(USART3, byte);
}

UART_buff *get_uart_buffer(uint32_t UART){
	switch(UART){
		case USART1:
			return &RX_buffer[0];
		break;
		case USART2:
			return &RX_buffer[1];
		break;
		case USART3:
			return &RX_buffer[2];
		break;
		default: // error - return
			return NULL;
	}
	return NULL;
}

/**
 * Check buffers for non-empty & run parsing function
 * @param UART - device to check
 */
void check_and_parce_UART(uint32_t UART){
	sendfun sf;
	static int oldlen = 0;
	UART_buff *curbuff = get_uart_buffer(UART);
	uint8_t datalen; // length of data in buffer - here we use param "end"
	if(!curbuff) return;
	switch(UART){
		case USART1:
			sf = uart1_send;
		break;
		case USART2:
			sf = uart2_send;
		break;
		case USART3:
			sf = uart3_send;
		break;
		default: // error - return
			return;
	}
	datalen = curbuff->end;
	if(!datalen) return; // buffer is empty
	if(oldlen != datalen){ // run checking buffer contents only when data size changed
		// process data and zero counter if all OK
		oldlen = curbuff->end = parce_incoming_buf((char*)curbuff->buf, datalen, sf);
	}
}

/**
 * Fill data in RX buffer to prepare it for further work
 * we don't use "start" parameter here, it's 0 always
 * @param UART - device to fill buffer
 * @param byte - data byte
 */
void fill_uart_RXbuff(uint32_t UART, uint8_t byte){
	UART_buff *curbuff;
	uint8_t bufidx;
	switch(UART){
		case USART1:
			bufidx = 0;
		break;
		case USART2:
			bufidx = 1;
		break;
		case USART3:
			bufidx = 2;
		break;
		default: // error - return
			return;
	}
	curbuff = &RX_buffer[bufidx];
	if(curbuff->end == UART_TX_DATA_SIZE){ // end of buffer - forget about data
		return;
	}
	curbuff->buf[curbuff->end++] = byte; // put byte into buffer
}

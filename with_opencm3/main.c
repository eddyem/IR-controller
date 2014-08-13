/*
 * main.c
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
#include "hardware_ini.h"
#include "cdcacm.h"
#include "uart.h"
#include "spi.h"

volatile uint32_t Timer = 0, tOVRFL = 0; // global timer (milliseconds), overflow counter
usbd_device *usbd_dev;

uint8_t ADC_monitoring = 0; // ==1 to make continuous monitoring

uint32_t ad7794_on = 0;
uint32_t ad7794_values[TRD_NO];
uint8_t doubleconv = 1; // ==0 to single conversion; 1 to double (with currents reversing)
#define ADC_direct()  setup_AD7794(EXTREFIN_1 | REF_DETECTION | UNIPOLAR_CODING, IEXC_DIRECT  | IEXC_1MA)
#define ADC_reverse() setup_AD7794(EXTREFIN_1 | REF_DETECTION | UNIPOLAR_CODING, IEXC_SWAPPED | IEXC_1MA)
/**
 * reads next value of voltage on TRD
 * function calls from anywhere
 * internal counter step counts number of operation: because of very long
 *    work of ADC function should be called several times to measure all
 *
 * @param doubleconv == 1 to do double conversion (with currents reversing)
 */
void read_next_TRD(){
	static uint8_t step = 0; // step of operation
	static uint8_t N = 0;    // number of current device
	static uint32_t val0 = 0, val1 = 0; // readed values
	// "default" in switch will process last step
	switch (step){ // now we should do something depending on current step value
		case 0: // step 0: set address
			if(N == 0) AD7794_init(); // reset ADC in beginning of each set
			GPIO_BSRR(GPIOC) = N << 6; // set address
			GPIO_BSRR(GPIOC) = GPIO10; // enable com
			N++; // and increment TRD counter
			if(N == TRD_NO) N = 0;
			val0 = val1 = 0;
			step++;
		break;
		case 1: // step 1: prepare reading in 1st current direction or single reading
			if(!val0){ // 1st value isn't ready yet
				if(!ADC_direct()){ // error: we can't setup reading
					ad7794_values[N] = 0;
					ad7794_on = 0;
					step++;
					break;
				}
				val0 = AD7794_NOTRDY;
			}
			if(val0 == AD7794_NOTRDY){ // data not ready yet
				val0 = read_AD7794(0);
				if(!val0 || val0 == AD7794_NOTRDY) break; // we'll process error in next time
				// All OK, prepare second reading
				if(doubleconv){ // double conversion -> prepare
					if(!ADC_reverse()){ // we can't setup it, but we have 1st value
						val1 = val0;
					}else{
						val1 = read_AD7794(0); // process it later
					}
				}else{ // simply copy value instead of multiplying by 2
					val1 = val0;
				}
			}else{ // val0 ready, check val1
				if(doubleconv && !val1){ // error
					ad7794_values[N] = 0;
					ad7794_on = 0;
					step++;
					break;
				}
				if(doubleconv && (val1 == AD7794_NOTRDY)){ // val1 not ready yet
					val1 = read_AD7794(0);
				}else{ // all OK, we can put sum of val0 & val1 into array
					ad7794_values[N] = val0 + val1;
					step++;
				}
			}
		break;
		default: // last step - turn off multiplexer
			step = 0;
			GPIO_BSRR(GPIOC) = (ADC_ADDR_MASK | GPIO10) << 16; // disable com & clear address bytes
	}
}

void AD7794_init(){
	uint8_t  i;
	ad7794_on = 0;
	i = reset_AD7794();
	if(i != ADC_NO_ERROR){
		if(i == ADC_ERR_NO_DEVICE){
			MSG("ADC signal is absent! Check connection.\r\n");
		}
	}else{
		if(!setup_AD7794(INTREFIN | REF_DETECTION | UNIPOLAR_CODING, IEXC_DIRECT  | IEXC_1MA)
			|| !AD7794_calibration(0)){
			MSG("Error: can't initialize AD7794!\r\n");
		}else{
			ad7794_on = 1;
			DBG("ADC ready\r\n");
		}
	}
}

int main(){
	int i;
	uint32_t Old_timer = 0, lastTRDread = 0, lastTmon = 0;
	//SPI_read_status SPI_stat;

	//rcc_clock_setup_in_hsi_out_48mhz();
	// RCC clocking: 8MHz oscillator -> 72MHz system
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	// GPIO
	GPIO_init();
	gpio_set(GPIOC, GPIO11); // turn off USB
	gpio_clear(GPIOC, GPIO12); // turn on LED

	// init USART1
	UART_init(USART1);

	// USB
	usbd_dev = USB_init();

	// init ADC
	ADC_init();

	// SysTick is a system timer with 1mc period
	SysTick_init();

	SPI1_init();

	// wait a little and then turn on USB pullup
	for (i = 0; i < 0x800000; i++)
		__asm__("nop");
	gpio_clear(GPIOC, GPIO11);
	ADC_calibrate_and_start();

	while(1){
		usbd_poll(usbd_dev);
		if(usbdatalen){ // there's something in USB buffer
			parce_incoming_buf(usbdatabuf, usbdatalen, usb_send);
			usbdatalen = 0; // all data have been processed - prepare to get new portion
		}
		check_and_parce_UART(); // also check data in UART buffers
		if(ad7794_on){
			if(Timer != lastTRDread){ // run this not more than once in 1ms
				lastTRDread = Timer;
				read_next_TRD();
			}
		}
		if(Timer - Old_timer > 999){ // write out time in seconds
			Old_timer += 1000;
			gpio_toggle(GPIOC, GPIO12); // toggle LED
			if(!ad7794_on) AD7794_init(); // try to init ADC
			//print_int(Timer/1000, usb_send);
		}else if(Timer < Old_timer){ // Timer overflow
			Old_timer = 0;
			tOVRFL++; // this is an overflow counter - for workinkg in long-long time interval
		}

		if((Timer - lastTmon > 9999) || (Timer < lastTmon)){ // run constant monitoring of ADC values each 10 seconds
			lastTmon += 10000;
			if(ADC_monitoring){
				print_time(lastsendfun);
				lastsendfun(' ');
				for(i = 0; i < 8; i++){
					print_int(ADC_value[i], lastsendfun);
					lastsendfun(' ');
				}
				print_ad_vals(lastsendfun);
			}
		}
	}
}


/**
 * SysTick interrupt: increment global time & send data buffer through USB
 */
void sys_tick_handler(){
	Timer++;
	usbd_poll(usbd_dev);
	usb_send_buffer();
}

// pause function, delay in ms
void Delay(uint16_t _U_ time){
	uint32_t waitto = Timer + time;
	while(Timer < waitto);
}

/**
 * print current time in milliseconds: 4 bytes for ovrvlow + 4 bytes for time
 * with ' ' as delimeter
 */
void print_time(sendfun s){
	print_int(tOVRFL, s);
	s(' ');
	print_int(Timer, s);
}

// D = dlmread('file')

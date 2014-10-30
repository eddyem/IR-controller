/*
 * spi.c
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
#include "spi.h"
#include "hardware_ini.h"

// Rx/Tx buffer
volatile uint8_t SPI_TxBuffer[SPI_BUFFERSIZE];
volatile uint8_t SPI_RxBuffer[SPI_BUFFERSIZE];
#ifndef SPI_USE_DMA
volatile uint8_t SPI_RxIndex   = 0; // omit first byte
volatile uint8_t SPI_TxIndex   = 0;
volatile uint8_t SPI_datalen   = 0;
#endif
volatile uint8_t SPI_EOT_FLAG = 1; // end of transmission flag, set by interrupt


uint32_t Current_SPI = SPI1; // this is SPI interface which would b
/**
 * Set current SPI to given value
 */
void switch_SPI(uint32_t SPI){
	Current_SPI = SPI;
}

void SPI_init(){
	switch(Current_SPI){
		case SPI1:
			SPI1_init();
		break;
		case SPI2:
			SPI2_init();
		break;
		default:
		return; // error
	}
}

//uint32_t read_end; // read timeout
/**
 * Write data to current SPI
 * @param data - buffer with data
 * @param len  - buffer length (<= DMA_BUFFERSIZE)
 * @return 0 in case of error (or 1 in case of success)
 */
uint8_t write_SPI(uint8_t *data, uint8_t len){
	//DBG("Write SPI.."); //return 1;
	uint8_t i;
	uint32_t tend = Timer + 10; // we will wait for end of previous transmission not more than 10ms
	//DBG("check\r\n");
	while(!SPI_EOT_FLAG && Timer < tend); // wait for previous DMA interrupt
	if(!SPI_EOT_FLAG){
		DBG("SPI error: no EOT flag!\r\n");
		return 0; // error: there's no receiver???
	}
	if(len > SPI_BUFFERSIZE) len = SPI_BUFFERSIZE;
	SPI_EOT_FLAG = 0;
	//DBG("OK\r\n");
	//read_end = Timer + 100; // we will wait for end of previous transmission not more than 0.1s
	for(i = 0; i < len; i++)
		SPI_TxBuffer[i] = data[i];
#ifdef SPI_USE_DMA
	DMA_Cmd(SPIx_TX_DMA_STREAM, ENABLE);
#else // !SPI_USE_DMA
	SPI_RxIndex = 0;
	SPI_TxIndex = 0;
	SPI_datalen = len; // set length of data to transmit
	// start transmission - enable interrupts
	SPI_CR2(Current_SPI) |= SPI_CR2_TXEIE | SPI_CR2_RXNEIE; //spi_enable_rx_buffer_not_empty_interrupt(SPI1); spi_enable_tx_buffer_empty_interrupt(SPI1);
	// Enable the SPI peripheral
	spi_enable(Current_SPI);
#endif // SPI_USE_DMA
	return 1;
}

/**
 * read data from SPI
 * @param data - buffer to which data will be copied
 * @param len - length of waiting data
 * @return buffer with data or NULL
 */
uint8_t *read_SPI(uint8_t *data, uint8_t len){
	//DBG("read SPI.. "); //return NULL;
	uint8_t i;
	uint32_t tend = Timer + 100; // we will wait for end of previous transmission not more than 0.1s
	//DBG("check\r\n");
	while((!SPI_EOT_FLAG || len != SPI_RxIndex) && Timer < tend);
	if(len != SPI_RxIndex){
		//DBG("SPI error: bad data length\r\n");
		return NULL;
	}
	//DBG("OK\r\n");
	for(i = 0; i < len; i++){
		data[i] = SPI_RxBuffer[i];
		//print_int(SPI_RxBuffer[i], usb_send);
	}
	//read_end = 0; // clear timeout: it mean that now we shouldn't wait any data
	return data;
}

/**
 * Check SPI read complete
 * @return reading status:
 * 		SPI_NO_DATA    - data isn't ready yet
 * 		SPI_DATA_READY - all OK, we can read buffer
 * 		SPI_READ_ERROR - there have been error in data receiving
 *
SPI_read_status check_SPI(){
	if(!read_end) return SPI_NO_DATA; // there's nothing to read
	if(!SPI_EOT_FLAG){
		if(Timer > read_end){// read timeout
			read_end = 0;
			return SPI_READ_ERROR; // timeout
		}else
			return SPI_NO_DATA;
	}
	return SPI_DATA_READY; // all OK: timeout haven't been reached yet
}
*/

// SPI interrupt
void spi_isr(uint32_t spi){
	// RX not empty
	if(SPI_SR(spi) & SPI_SR_RXNE){
		if(SPI_RxIndex < SPI_datalen){ // we haven't receive all data yet
		// Receive Transaction data
			SPI_RxBuffer[SPI_RxIndex++] = SPI_DR(spi);
		}else{ // disable RXNE interrupt & turn off SPI
			spi_disable_rx_buffer_not_empty_interrupt(spi);
			spi_disable(spi);
		}
	}
	// TX empty
	if(SPI_SR(spi) & SPI_SR_TXE){
		if(SPI_TxIndex < SPI_datalen){ // buffer still not sent fully
		// Send Transaction data
			SPI_DR(spi) = SPI_TxBuffer[SPI_TxIndex++];
		}else{ // disable TXE interrupt + set EOT flag
			spi_disable_tx_buffer_empty_interrupt(spi);
			SPI_EOT_FLAG = 1;
		}
	}
	SPI_SR(spi) = 0; // clear all interrupt flags
}

// interrupts for SPI1 & SPI2
void spi1_isr(){
	spi_isr(SPI1);
}

void spi2_isr(){
	spi_isr(SPI2);
}

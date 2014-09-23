/*
 * onewire.c - functions to work with 1-wire devices
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
#include "onewire.h"

#define OW_0	0x00
#define OW_1	0xff
#define OW_R	0xff
#define OW_RST	0xf0


// In/Out buffer
uint8_t ow_buf[8];
/**
 * this function sends bits of ow_byte (LSB first) to 1-wire line
 * @param ow_byte - byte to convert
 * @param Nbits   - number of bits to send
 */
void OW_SendBits(uint8_t ow_byte, uint8_t Nbits){
	uint8_t i, byte;
	if(Nbits == 0) return;
	if(Nbits > 8) Nbits = 8;
	for(i = 0; i < Nbits; i++){
		if(ow_byte & 0x01){
			byte = OW_1;
		}else{
			byte = OW_0;
		}
		fill_uart_buff(OW_USART_X, byte); // send next "bit"
		ow_byte = ow_byte >> 1;
	}
}

/*
 * Inverce conversion - read data (not more than 8 b
 */
uint8_t OW_ReadByte(){
	UART_buff *curbuff = get_uart_buffer(OW_USART_X);
	uint8_t ow_byte = 0, i, L, *buf;
	if(!curbuff || !(L = curbuff->end)) return 0; // no data?
	if(L > 8) L = 8; // forget all other data
	buf = curbuff->buf;
	for(i = 0; i < L; i++, buf++){
		ow_byte = ow_byte >> 1; // prepare for next bit filling
		if(*buf == OW_1){
			ow_byte |= 0x80; // MSB = 1
		}
	}
	return ow_byte >> (8 - L); // shift to the end: L could be != 8 ???
}


/*
 * Configure peripherial ports (USART2) for 1-wire
  */
void OW_Init(){
	struct usb_cdc_line_coding owlc = {
		.dwDTERate   = 115200,
		.bCharFormat = USB_CDC_1_STOP_BITS,
		.bParityType = USB_CDC_NO_PARITY,
		.bDataBits   = 8,
	};
	UART_init(OW_USART_X);
	UART_setspeed(OW_USART_X, &owlc);
}

/*
 * 1-wire reset
 * Reset procedure: USART settings are 9600,8,n,1,
 *     send 0xf0 then check what we get
 *     if not 0xf0 line is busy.
 * Other operations work with next USART settings: 115200,8,n,1
 *
 * return 1 in case of 1-wire devices present; otherwise return 0
 */
uint8_t OW_Reset() {
	uint8_t ow_presence;
	UART_buff *curbuff;
	// change speed to 9600
	usart_set_baudrate(OW_USART_X, 9600);
	//USART_ClearFlag(OW_USART_X, USART_FLAG_TC);
	fill_uart_buff(OW_USART_X, OW_RST); // send 1 byte data
	// wait for end of transmission
	while(!(USART_SR(OW_USART_X) & USART_SR_TC));
	curbuff = get_uart_buffer(OW_USART_X);
	if(!curbuff || !(curbuff->end)) return 0; // error reading
	curbuff->end = 0; // zero counter
	ow_presence = curbuff->buf[0];
	// change speed back
	usart_set_baudrate(OW_USART_X, 115200);
	// if there is any device on bus, it will pull it, so we'll get not 0xf0
	if(ow_presence != OW_RST){
		return 1;
	}
	// we get 0xf0 -> there's nothing on the bus
	return 0;
}

/*
 * Procedure of 1-wire communications
 * variables:
 * 		sendReset - send RESET before transmission
 * 		command - bytes sent to the bus (if we want to read, send OW_READ_SLOT)
 * 		cLen - command buffer length (how many bytes to send)
 * 		data - pointer for reading buffer (if reading needed)
 * 		readStart - first byte to read (starts from 0) or OW_NO_READ (not read)
 *
 * return 1 if succeed, 0 if failure
 */
uint8_t OW_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen,
		uint8_t *data, uint8_t dLen, uint8_t readStart) {
	// if reset needed - send RESET and check bus
	if(sendReset){
		if(OW_Reset() == 0){
			return 0;
		}
	}
	while(cLen > 0){
		OW_SendBits(*command, 8);
		command++;
		cLen--;
		// wait for EOT
		while(!(USART_SR(OW_USART_X) & USART_SR_TC));
		// put data from bus into user buffer
		if(readStart == 0 && dLen > 0){
			*data = OW_ReadByte();
			data++;
			dLen--;
		}else{
			if(readStart != OW_NO_READ){
				readStart--;
			}
		}
	}
	return 1;
}

/*
 * scan 1-wire bus
 * 		num - max number of devices
 * 		buf - array for devices' ID's (8*num bytes)
 * return amount of founded devices
 */
uint8_t OW_Scan(uint8_t *buf, uint8_t num) {
	unsigned long path,next,pos;
	uint8_t bit,chk;
	uint8_t cnt_bit, cnt_byte, cnt_num;
	path=0;
	cnt_num=0;
	do{
		//(issue the 'ROM search' command)
		if( 0 == OW_WriteCmd(OW_SEARCH_ROM) ) return 0;
		next=0; // next path to follow
		pos=1;  // path bit pointer
		for(cnt_byte = 0; cnt_byte != 8; cnt_byte++){
			buf[cnt_num*8 + cnt_byte] = 0;
			for(cnt_bit = 0; cnt_bit != 8; cnt_bit++){
				//(read two bits, 'bit' and 'chk', from the 1-wire bus)
				OW_SendBits(OW_R, 2);
				bit = OW_ReadByte();
				chk = bit & 0x02; // bit 1
				bit = bit & 0x01; // bit 0
				//bit = (ow_buf[0] == OW_1); chk = (ow_buf[1] == OW_1);
				if(bit && chk) return 0; // error
				if(!bit && !chk){ // collision, both are zero
					if (pos & path) bit = 1;     // if we've been here before
					else next = (path&(pos-1)) | pos; // else, new branch for next
					pos <<= 1;
				}
				//(save this bit as part of the current ROM value)
				if (bit) buf[cnt_num*8 + cnt_byte]|=(1<<cnt_bit);
				//(write 'bit' to the 1-wire bus)
				OW_SendBits(bit, 1);
			}
		}
		//(output the just-completed ROM value)
		path=next;
		cnt_num++;
	}while(path && cnt_num < num);
	return cnt_num;
}

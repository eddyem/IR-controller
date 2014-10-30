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


uint8_t dev_amount = 0;   // amount of 1-wire devices
uint8_t ID_buf[64] = {0}; // 1-wire devices ID buffer (8 bytes for every device)
uint8_t NUM_buf[8] = {0}; // numerical identificators for each sensor

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

void OW_ClearBuff(){
	UART_buff *curbuff = get_uart_buffer(OW_USART_X);
	curbuff->end = 0;
}

/*
 * Inverce conversion - read data (not more than 8 bits)
 */
uint8_t OW_ConvertByte(uint8_t *bits, uint8_t L){
	uint8_t ow_byte = 0, i, *st = bits;
	if(L > 8) L = 8; // forget all other data
	for(i = 0; i < L; i++, st++){
		ow_byte = ow_byte >> 1; // prepare for next bit filling
		if(*st == OW_1){
			ow_byte |= 0x80; // MSB = 1
		}
	}
	ow_byte >>= (8 - L);
print_hex(bits, L, lastsendfun);
lastsendfun(' ');
print_hex(&ow_byte, 1, lastsendfun);
newline(lastsendfun);
	return ow_byte; // shift to the end: L could be != 8 ???
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
uint8_t OW_Reset(){
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

/**
 * Procedure of 1-wire communications
 * variables:
 * @param sendReset - send RESET before transmission
 * @param command - bytes sent to the bus (if we want to read, send OW_READ_SLOT)
 * @param cLen - command buffer length (how many bytes to send)
 * @return 1 if succeed, 0 if failure
 */
uint8_t OW_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen){
	// if reset needed - send RESET and check bus
	if(sendReset){
		if(OW_Reset() == 0){
			return 0;
		}
	}
	while(cLen-- > 0){
		OW_SendBits(*command, 8);
		command++;
	}
	return 1;
}

/**
 * Check USART IN buffer for ready & fill user buffer with data on success
 * @param buflen - expected buffer length
 * @param data - pointer for reading buffer (if reading needed must be at least buflen-readStart bytes)
 * @param readStart - first byte to read (starts from 0) or OW_NO_READ (not read)
 * @return 0 if buffer not ready; 1 if OK
 */
uint8_t OW_Get(uint8_t buflen, uint8_t *data, uint8_t readStart){
	UART_buff *curbuff = get_uart_buffer(OW_USART_X);
	uint8_t *buff = curbuff->buf;
	if(curbuff->end < buflen/8) return 0;
	while(buflen-- > 0){
		if(readStart == 0){
			*data++ = OW_ConvertByte(buff, 8);
		}else{
			if(readStart != OW_NO_READ){
				readStart--;
			}
		}
		buff += 8;
	}
	curbuff->end = 0; // zero counter
	return 1;
}

/*
 * scan 1-wire bus
 * WARNING! The procedure works in real-time, so it is VERY LONG
 * 		num - max number of devices
 * 		buf - array for devices' ID's (8*num bytes)
 * return amount of founded devices
 *
uint8_t OW_Scan(uint8_t *buf, uint8_t num){
	unsigned long path,next,pos;
	uint8_t bit,chk;
	uint8_t cnt_bit, cnt_byte, cnt_num;
	path=0;
	cnt_num=0;
	do{
		//(issue the 'ROM search' command)
		if( 0 == OW_WriteCmd(OW_SEARCH_ROM) ) return 0;
		OW_Wait_TX();
		OW_ClearBuff(); // clear RX buffer
		next = 0; // next path to follow
		pos = 1;  // path bit pointer
		for(cnt_byte = 0; cnt_byte != 8; cnt_byte++){
			buf[cnt_num*8 + cnt_byte] = 0;
			for(cnt_bit = 0; cnt_bit != 8; cnt_bit++){
				//(read two bits, 'bit' and 'chk', from the 1-wire bus)
				OW_SendBits(OW_R, 2);
				OW_Wait_TX();
				bit = -----OW_ReadByte();
				chk = bit & 0x02; // bit 1
				bit = bit & 0x01; // bit 0
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
				OW_Wait_TX();
			}
		}
		path=next;
		cnt_num++;
	}while(path && cnt_num < num);
	return cnt_num;
}*/

uint8_t OW_Scan(uint8_t *buf, uint8_t num){
	uint8_t flg, b[11], i;
	flg = OW_Send(1, (uint8_t*)"\xcc\x33\xff\xff\xff\xff\xff\xff\xff\xff\xff", 11);
	if(!flg) return 0;
	OW_Wait_TX();
	if(!OW_Get(11, b, 0)) return 0;
	num += 2;
	for(i = 2; i < num; i++) *buf++ = b[i];
	return 1;
}


//OW_USART_X
/*
void OW_getTemp(){
	uint8_t buf[9], i;
	void printTBuf(){
		uint8_t j;
		OW_Send(0, (uint8_t*)"\xbe\xff\xff\xff\xff\xff\xff\xff\xff\xff", 10, buf, 9, 1);
			for(j = 0; j != 9; j++)
				printInt(&buf[j], 1);
		newline();
	}
	// send broadcast message to start measurement
	if(!OW_Send(1, (uint8_t*)"\xcc\x44", 2)) return;
	Delay(1000);
	// read values
	if(dev_amount == 1){
		if(OW_WriteCmd(OW_SKIP_ROM)) printTBuf();
	}else{
		for(i = 0; i < dev_amount; i++){
			MSG("Device ", "ow");
			USB_Send_Data(i + '0');
			MSG(": ", 0);
			if(OW_WriteCmd(OW_MATCH_ROM)){
				OW_SendOnly(0, &ID_buf[i*8], 8);
				printTBuf();
			}
		}
	}
}
*/

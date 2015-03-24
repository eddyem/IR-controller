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
#include "user_proto.h"

OW_ID id_array[OW_MAX_NUM]; // 1-wire devices ID buffer (not more than eight)
uint8_t dev_amount = 0;   // amount of 1-wire devices

// states of 1-wire processing queue
typedef enum{
	OW_OFF_STATE,    // not working
	OW_RESET_STATE,  // reset bus
	OW_SEND_STATE,   // send data
	OW_READ_STATE,   // wait for reading
} OW_States;

OW_States OW_State = OW_OFF_STATE; // 1-wire state, 0-not runned
uint8_t OW_wait_bytes = 0;   // amount of bytes needed to read
uint8_t OW_start_idx = 0;    // starting index to read from 1-wire buffer
uint8_t *read_buf = NULL;    // buffer to read

uint8_t ow_data_ready = 0;   // flag of reading OK

void OW_printID(uint8_t N, sendfun s){
	void putc(uint8_t c){
		if(c < 10)
			s(c + '0');
		else
			s(c + 'a' - 10);
	}
	int i;
	uint8_t *b = id_array[N].bytes;
	s('0'); s('x'); // prefix 0x
	for(i = 0; i < 8; i++){
		putc(b[i] >> 4);
		putc(b[i] & 0x0f);
	}
	s('\n');
}

uint8_t ow_was_reseting = 0;

void OW_process(){
	switch(OW_State){
		case OW_OFF_STATE:
			return;
		break;
		case OW_RESET_STATE:
			OW_State = OW_SEND_STATE;
			ow_was_reseting = 1;
			ow_reset();
			//MSG("reset\n");
		break;
		case OW_SEND_STATE:
			if(!OW_READY()) return; // reset in work
			if(ow_was_reseting){
				if(!OW_get_reset_status()){
					BYTE_MSG("error: no 1-wire devices found\n");
					ow_was_reseting = 0;
				//	OW_State = OW_OFF_STATE;
				//	return;
				}
			}
			ow_was_reseting = 0;
			OW_State = OW_READ_STATE;
			run_dmatimer(); // turn on data transfer
			//MSG("send\n");
		break;
		case OW_READ_STATE:
			if(!OW_READY()) return; // data isn't ready
			OW_State = OW_OFF_STATE;
			adc_dma_on(); // return DMA1_1 to ADC at end of data transmitting
			if(read_buf){
				read_from_OWbuf(OW_start_idx, OW_wait_bytes, read_buf);
			}
			ow_data_ready = 1;
			//MSG("read\n");
		break;
	}
}

/**
 * fill Nth array with identificators
 */
//uint8_t comtosend = 0;
void OW_fill_ID(uint8_t N){
	if(N >= OW_MAX_NUM){
		BYTE_MSG("number too big\n");
		return;
	}
	//OW_Send(1, (uint8_t*)"\xcc\x33", 2);
	OW_Send(1, (uint8_t*)"\x19", 1);
//	OW_Send(1, &comtosend, 1);
//	comtosend++;
	//OW_Send(1, (uint8_t*)"\xcc\xbe", 2);
	OW_add_read_seq(9); // wait for 9 bytes
	//OW_Send(0, (uint8_t*)"\xcc\x33\x10\x45\x94\x67\x7e\x8a", 8);
	read_buf = id_array[N].bytes;
	OW_wait_bytes = 8;
	OW_start_idx = 0;
/*
	OW_Send(0, (uint8_t*)"\x99\xee", 2);
	OW_wait_bytes = 2;
	OW_start_idx = 0;
	read_buf = id_array[N].bytes;
*/
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
	uint8_t f = 1;
	ow_data_ready = 0;
	// if reset needed - send RESET and check bus
	if(sendReset)
		OW_State = OW_RESET_STATE;
	else
		OW_State = OW_SEND_STATE;
	while(cLen-- > 0){
		if(!OW_add_byte(*command, 8, f)) return 0;
		command++;
		f = 0;
	}
	return 1;
}


#if 0


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

#endif

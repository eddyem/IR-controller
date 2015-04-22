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
#include "includes.h"

#define OW_0	0x00
#define OW_1	0xff
#define OW_R	0xff

// In/Out buffer
uint8_t ow_buf[8];
/*
 * this function converts 1byte to 8 to transmit through USART
 * ow_byte - byte to convert
 */
void OW_toBits(uint8_t ow_byte){
	uint8_t i;
	uint8_t *ow_bits = ow_buf;
	for(i = 0; i < 8; i++){
		if(ow_byte & 0x01){
			*ow_bits = OW_1;
		}else{
			*ow_bits = OW_0;
		}
		ow_bits++;
		ow_byte = ow_byte >> 1;
	}
}

/*
 * Inverce conversion
 */
uint8_t OW_toByte() {
	uint8_t ow_byte = 0, i;
	uint8_t *ow_bits = ow_buf;
	for (i = 0; i < 8; i++) {
		ow_byte = ow_byte >> 1;
		if(*ow_bits == OW_1){
			ow_byte |= 0x80;
		}
		ow_bits++;
	}
	return ow_byte;
}


/*
 * Configure peripherial ports (USART2) for 1-wire
  */
void OW_Init(){
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(OW_USART_X, &USART_InitStructure);
	USART_Cmd(OW_USART_X, ENABLE);
	/*
	 * turn on DMA1, USART2 is on DMA channels 6(RX) & 7(TX)
	 * USART3 is on channels 2 & 3
	 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/*
 * 1-wire reset
 * Reset procedure: USART settings are 9600,8,n,1,
 *     send 0xf0 then check what we get
 *     if not 0xf0 line is busy.
 * Other operations work with next USART settings: 115200,8,n,1
 */
uint8_t OW_Reset() {
	uint8_t ow_presence;
	USART_InitTypeDef USART_InitStructure;
	// change speed to 9600
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(OW_USART_X, &USART_InitStructure);
	USART_ClearFlag(OW_USART_X, USART_FLAG_TC);
	USART_SendData(OW_USART_X, 0xf0);
	// wait for end of transmission
	while (USART_GetFlagStatus(OW_USART_X, USART_FLAG_TC) == RESET);
	ow_presence = USART_ReceiveData(OW_USART_X);
	// change speed back
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(OW_USART_X, &USART_InitStructure);
	// if there is any device on bus, it will pull it, so we'll get not 0xf0
	if (ow_presence != 0xf0) {
		return 1;
	}
	// we get 0xf0 -> there's nothing on the bus
	return 0;
}

// send nbits bits from ow_buf to 1-wire
void OW_SendBits(uint8_t nbits){
	DMA_InitTypeDef DMA_InitStructure;
	// DMA for reading
	DMA_DeInit(OW_DMA_RX_CHANNEL);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(OW_USART_X->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = nbits;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(OW_DMA_RX_CHANNEL, &DMA_InitStructure);

	// DMA for writting
	DMA_DeInit(OW_DMA_TX_CHANNEL);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(OW_USART_X->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = nbits;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(OW_DMA_TX_CHANNEL, &DMA_InitStructure);

	// start send cycle
	USART_ClearFlag(OW_USART_X, USART_FLAG_RXNE | USART_FLAG_TC | USART_FLAG_TXE);
	USART_DMACmd(OW_USART_X, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(OW_DMA_RX_CHANNEL, ENABLE);
	DMA_Cmd(OW_DMA_TX_CHANNEL, ENABLE);

	// wait end of transmission
	while OW_DMA_TRANSFER_END;

	// turn off DMA
	DMA_Cmd(OW_DMA_TX_CHANNEL, DISABLE);
	DMA_Cmd(OW_DMA_RX_CHANNEL, DISABLE);
	USART_DMACmd(OW_USART_X, USART_DMAReq_Tx | USART_DMAReq_Rx, DISABLE);
}

/*
 * Procedure of 1-wire communications
 * variables:
 * 		sendReset - send RESET before transmission
 * 		command - bytes sent to the bus (if we want to read, send OW_READ_SLOT)
 * 		cLen - command buffer length (how many bytes to send)
 * 		data - pointer for reading buffer (if reading needed)
 * 		readStart - first byte to read (starts from 0) or OW_NO_READ (not read)
 */
uint8_t OW_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen,
		uint8_t *data, uint8_t dLen, uint8_t readStart) {
	// if reset needed - send RESET and check bus
	if (sendReset) {
		if (OW_Reset() == 0) {
			return 0;
		}
	}
	while (cLen > 0) {
		OW_toBits(*command);
		command++;
		cLen--;
		OW_SendBits(8);
		// put data from bus into user buffer
		if (readStart == 0 && dLen > 0) {
			*data = OW_toByte();
			data++;
			dLen--;
		} else {
			if (readStart != OW_NO_READ) {
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
		if( 0 == OW_WriteCmd(OW_SEARCH_ROM)) return 0;
		next=0; // next path to follow
		pos=1;  // path bit pointer
		for (cnt_byte=0; cnt_byte!=8; cnt_byte++){
			buf[cnt_num*8 + cnt_byte] = 0;
			for (cnt_bit=0; cnt_bit!=8; cnt_bit++){
				//(read two bits, 'bit' and 'chk', from the 1-wire bus)
				OW_toBits(OW_R);
				OW_SendBits(2);
				bit = (ow_buf[0] == OW_1); chk = (ow_buf[1] == OW_1);
				if(bit && chk) return 0; // error
				if(!bit && !chk){ // collision, both are zero
						if(pos&path) bit=1;     // if we've been here before
						else next=(path&(pos-1))|pos; // else, new branch for next
						pos<<=1;
					}
				//(save this bit as part of the current ROM value)
				if (bit) buf[cnt_num*8 + cnt_byte]|=(1<<cnt_bit);
				//(write 'bit' to the 1-wire bus)
				OW_toBits(bit);
				OW_SendBits(1);
			}
		}
		//(output the just-completed ROM value)
		path=next;
		cnt_num++;
	}while(path && cnt_num < num);
	return cnt_num;
}

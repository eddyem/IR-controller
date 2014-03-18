/*
 * main.c
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

uint8_t dev_amount = 0;  // amount of 1-wire devices
uint8_t ID_buf[64];      // 1-wire devices ID buffer (8 bytes for every device)

int main(void){
	//SystemInit(); // If won't work - comment this line
	Set_System();
	LED_OnBlink();
	findDev();
	printDev();
	while(1){
		if(dev_amount){
			if(FLAGS & FLAG_READ_T){
				getTemp(); FLAGS ^= FLAG_READ_T;
			}
			if(FLAGS & FLAG_GETDEV){
				findDev(); FLAGS ^= FLAG_GETDEV;
			}
			if(FLAGS & FLAG_PRINTDEV){
				printDev(); FLAGS ^= FLAG_PRINTDEV;
			}
		}
		if(FLAGS & FLAG_PRINTHALL){
			if(Hall_GPIOx->IDR & Hall_PIN)
				MSG("clear", "HC");
			else
				MSG("magnet", "HM");
			newline();
			FLAGS ^= FLAG_PRINTHALL;
		}
		if(FLAGS & FLAG_SWTCH_OFF){
			FLAGS ^= FLAG_SWTCH_OFF;
			ADG_off();
		}
		if((FLAGS & FLAG_SWITCH_N) && Uval_ready){
			FLAGS ^= FLAG_SWITCH_N;
			if(User_value & 0xFFFFFFF0){
				MSG("Err ChN", "err"); newline();
			}else
				switch_Nth();
		}
		if(FLAGS & FLAG_PRINTADC){
			prntADC(); FLAGS ^= FLAG_PRINTADC;
		}
		if(FLAGS & FLAG_PRINTSPD){
			prntSPD(); FLAGS ^= FLAG_PRINTSPD;
		}
		if((FLAGS & FLAG_MOTOR) && Uval_ready){ // move motor && steps amount is readed
			FLAGS ^= FLAG_MOTOR;
			if(User_value > 0x10000000){ // max value: steps*16 must be an uint32_t !!!
				MSG("Err step cnt", "err");
				newline();
			}else
				move_motor();
		}
		if((FLAGS & FLAG_M_PERIOD) && Uval_ready){
			FLAGS ^= FLAG_M_PERIOD;
			if(User_value > (uint32_t)0xFFFF){
				MSG("Err m period", "err");
				newline();
			}else
				set_motor_period();
		}
		if(FLAGS & FLAG_ZAKWIRE){ // clear flag only after getting value or error
			if(ZW_status == ZW_OFF){
				ZW_start_measurement();
			}else if(ZW_status == ZW_ERR || ZW_status == ZW_DATARDY){
				switch(ZW_status){
					case ZW_ERR:
						MSG("Err zacwire", "err");
						newline();
					break;
					case ZW_DATARDY:
						MSG("ZacWire: ", 0);
						printInt((uint8_t*)&ZW_result, 2);
						newline();
					break;
				}
				ZW_status = ZW_OFF;
				FLAGS ^= FLAG_ZAKWIRE;
			}
		}
		Delay(100);
	}
}

inline void getTemp(){
	uint8_t buf[9], i;
	void printTBuf(){
		uint8_t j;
		OW_Send(0, (uint8_t*)"\xbe\xff\xff\xff\xff\xff\xff\xff\xff\xff", 10, buf, 9, 1);
			for(j = 0; j != 9; j++)
				printInt(&buf[j], 1);
		newline();
	}
	// send broadcast message to start measurement
	if(!OW_SendOnly(1, (uint8_t*)"\xcc\x44", 2)) return;
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

void findDev(){
	// find 1-wire devices
	dev_amount = OW_Scan(ID_buf, dev_amount);
	MSG("Found ", "owN");
	USB_Send_Data(dev_amount + '0');
	MSG(" devices", 0);
	newline();
}

void printDev(){
	uint8_t *ptr = ID_buf;
	uint8_t i, j;
	if(dev_amount == 0)
		MSG("No devices found", "err");
	// print IDs in binary
	for(i = 0; i < dev_amount; i++){
		MSG("device ", "ow");  USB_Send_Data(i + '0');
		MSG(":\r\n", "");
		for(j = 0; j != 8; j++){
			printInt(ptr++, 1);
		}
		newline();
	}
	newline();
}

inline void prntADC(){
	MSG("ADC value ", 0);
	printInt((uint8_t*)&ADC_value, 2);
	newline();
}

/**
 * Activate Nth output of ADG506A
 * N == User_value
 */
inline void switch_Nth(){
	uint32_t addr = (User_value & 0x0f) | 0x10;
	GPIOC->BRR = (uint32_t)0x1f; // first - clear all
	Delay(2); // wait a little
	GPIOC->BSRR = addr; // set new address
	if(MoreMessages){
		P("Switch to ");
		addr &= 0x0f;
		printInt((uint8_t*)&addr, 1);
		newline();
	}
}

/**
 * Turn off ADG506A
 */
inline void ADG_off(){
	GPIOC->BRR = (uint32_t)0x1f;
	if(MoreMessages){
		P("Switch off"); newline();
	}
}

/*
 * Print USART line speed
 */
inline void prntSPD(){
	MSG("Line speed: ", "chspd");
	printInt((uint8_t*)&linecoding.bitrate, 4);
	newline();
}

/*
 * System setup
 */
inline void Set_System(void){
	// SysTick setup: 1 ms / SYSTICK_PRESCALE
	SysTick_Config(SystemCoreClock / SYSTICK_PRESCALE / 1000);
	// RCC configuration
	RCC_Config();
	// Ports configuration
	Ports_Config();
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
	OW_Init();
}

/*
 * Send char array wrd thru USB
 */
void prnt(uint8_t *wrd){
	if(!wrd) return;
	while(*wrd) USB_Send_Data(*wrd++);
}

/**
 * Prints newline in "more messages" mode
 * else do nothing
 */
void newline(){
	if(MoreMessages) P("\r\n");
}

/**
 * Send to terminal an integer value
 * if "more messages" mode == 1, value will be in hex,
 * else value will be printed as BigEndian binary
 * @param val - value itself
 * @param len - bytelength of val
 */
void printInt(uint8_t *val, int8_t len){
	if(MoreMessages){
		P("0x");
		void putc(uint8_t c){
			if(c < 10)
				USB_Send_Data(c + '0');
			else
				USB_Send_Data(c + 'a' - 10);
		}
		while(--len >= 0){
			putc(val[len] >> 4);
			putc(val[len] & 0x0f);
		}
		USB_Send_Data(' ');
	}else{
		int8_t i;
		//while(--len >= 0)
		for(i = 0; i < len; i++)
			USB_Send_Data(val[i]);
	}
}

volatile uint16_t delay_counter = 0;

// Delay in ms
void Delay(uint16_t time){
	delay_counter = time;
	while(delay_counter != 0);
}
void Delay_SysTickHandler(){
	if(delay_counter > 0)
		delay_counter--;
}

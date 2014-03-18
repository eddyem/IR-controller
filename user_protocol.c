/*
 * user_protocol.c - user commands parser
 *
 * Copyright 2013 Edward V. Emelianoff <eddy@sao.ru>
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

/*
 * Messages level:
 * 0 - no addition messages (work with computer directly)
 * 1 - additional messages (work through character or line terminal)
 */
volatile uint8_t MoreMessages = 1;
// different internal flags
flag_t FLAGS = 0;
// integer value given by user
volatile int32_t User_value = 0;
// flag: ==1 when user value reading ends - for character terminals
uint8_t Uval_ready = 1;
uint16_t read_int(uint16_t start, uint16_t cnt);

/**
 * read user commands from USB buffer
 * @param cnt - number of symbols in buffer
 */
void usb_handle_command(uint16_t cnt){
	char *answer = NULL;
	uint8_t command;
	uint16_t i = 0;
	if(!Uval_ready) i += read_int(0, cnt);
	for(; i < cnt; i++){
		command = USB_Rx_Buffer[i];
		switch(command){
			case CMD_NO_MESGS:
				MoreMessages = 0;
				answer = NULL;
			break;
			case CMD_MORE_MESGS:
				MoreMessages = 1;
				answer = "Msgs on";
			break;
			case CMD_LED_ON:
				LED_On();
				answer = "On";
			break;
			case CMD_LED_OFF:
				LED_Off();
				answer = "Off";
			break;
			case CMD_LED_BLINK:
				LED_OnBlink();
				answer = "Blk";
			break;
			case CMD_LED_DUTY_PLUS:
				LED_DutyPlus();
				answer = "Shn";
			break;
			case CMD_LED_DUTY_MINUS:
				LED_DutyMinus();
				answer = "Fad";
			break;
			case CMD_1W_GET_TEMP:
				FLAGS |= FLAG_READ_T;
				answer = "Read T";
			break;
			case CMD_1W_GET_DEV:
				FLAGS |= FLAG_GETDEV;
				answer = "find devices";
			break;
			case CMD_1W_PRNT_DEV:
				FLAGS |= FLAG_PRINTDEV;
				answer = "Print devices";
			break;
			case CMD_HALL_GET:
				FLAGS |= FLAG_PRINTHALL;
				answer = "Print Hall";
			break;
			case CMD_SWTCH_N:
				FLAGS |= FLAG_SWITCH_N;
				i += read_int(i+1, cnt);
				answer = "SwtchN";
			break;
			case CMD_SWTCH_OFF:
				FLAGS |= FLAG_SWTCH_OFF;
				answer = "SwtchOff";
			break;
			case CMD_ADC_GET:
				FLAGS |= FLAG_PRINTADC;
				answer = "Print ADC val";
			break;
			case CMD_MOTOR0:
			case CMD_MOTOR1:
			case CMD_MOTOR2:
			case CMD_MOTOR3:
			case CMD_MOTOR4:
				i += read_int(i+1, cnt);
				answer = "Motor";
				FLAGS |= FLAG_MOTOR;
				Motor_number = command - (uint8_t)'0';
			break;
			case CMD_MOTOR_PERIOD:
				i += read_int(i+1, cnt);
				answer = "M period";
				FLAGS |= FLAG_M_PERIOD;
			break;
			case CMD_MOTOR_STOP:
				stop_timer1();
				answer = "M stop";
			break;
			case CMD_MOTOR_GETSTEP:
				printInt((uint8_t*)&Motor_steps, 4);
				answer = " steps/16";
			break;
			case CMD_ZAKWIRE:
				answer = "ZakWire";
				FLAGS |= FLAG_ZAKWIRE;
			break;
			default:
				answer = "Unk";
		}
		if(MoreMessages){
			P(answer);
			newline();
		}
	}
}

// sign of readed value
int32_t sign;
/**
 * Read from TTY integer value given by user (in DEC).
 *    Reading stops on first non-numeric symbol.
 *    To work with symbol terminals reading don't stops on buffer's end,
 *    it waits for first non-numeric char.
 *    When working on string terminals, terminate string by '\n', 0 or any other symbol
 * @param start - starting position in buffer
 * @param cnt - number of symbols in buffer
 * @return amount of readed symbols
 */
uint16_t read_int(uint16_t start, uint16_t cnt){
	uint16_t readed = 0, i;
	if(Uval_ready){ // this is first run
		Uval_ready = 0; // clear flag
		User_value = 0; // clear value
		sign = 1; // clear sign
	}
	if(!cnt) return 0;
	for(i = start; i < cnt; i++, readed++){
		uint8_t chr = USB_Rx_Buffer[i];
		if(chr == '-'){
			if(sign == 1){
				sign = -1;
				continue;
			}else{ // '-' after numbers
				Uval_ready = 1;
				break;
			}
		}
		if(chr < '0' || chr > '9'){
			Uval_ready = 1;
			break;
		}
		User_value = User_value * 10 + (int32_t)(chr - '0');
	}
	if(Uval_ready) // reading has met an non-numeric character
		User_value *= sign;
	return readed;
}

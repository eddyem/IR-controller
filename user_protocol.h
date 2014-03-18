/*
 * user_protocol.h - user defined commands & main flags
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

#pragma once
#ifndef __USER_PROTOCOL_H__
#define __USER_PROTOCOL_H__

#include "stm32f10x.h"

typedef uint32_t flag_t;
// flags
extern flag_t FLAGS;
// integer value given by user
extern volatile int32_t User_value;
// ==1 when user value readed
extern uint8_t Uval_ready;
void usb_handle_command(uint16_t cnt);

#define F(arg)  ((flag_t)arg)

#define FLAG_READ_T		F(0x0001)
#define FLAG_GETDEV		F(0x0002)
#define FLAG_PRINTDEV	F(0x0004)
#define FLAG_PRINTHALL	F(0x0008)
#define FLAG_PRINTADC	F(0x0010)
#define FLAG_PRINTSPD	F(0x0020)
#define FLAG_MOTOR		F(0x0040)
#define FLAG_M_PERIOD	F(0x0080)
#define FLAG_ZAKWIRE	F(0x0100)
#define FLAG_SWITCH_N	F(0x0200)
#define FLAG_SWTCH_OFF	F(0x0400)

// usb console commands
#define CMD_LED_ON				'<'
#define CMD_LED_OFF				'>'
#define CMD_LED_BLINK			'b'
#define CMD_LED_DUTY_PLUS		'+'
#define CMD_LED_DUTY_MINUS		'-'

#define CMD_1W_GET_TEMP			't'
#define CMD_1W_GET_DEV			'c'
#define CMD_1W_PRNT_DEV			'p'

#define CMD_HALL_GET			'h'

#define CMD_ADC_GET				'a'
#define CMD_SWTCH_N				'N'
#define CMD_SWTCH_OFF			'O'

#define CMD_ZAKWIRE				'z'

#define CMD_MORE_MESGS			'V'
#define CMD_NO_MESGS			'v'

// !!! motor commands are used in cmd parser to set motor number:
// !!!     Motor_number = command - '0';
#define CMD_MOTOR0				'0'
#define CMD_MOTOR1				'1'
#define CMD_MOTOR2				'2'
#define CMD_MOTOR3				'3'
#define CMD_MOTOR4				'4'
#define CMD_MOTOR_PERIOD		'S'
#define CMD_MOTOR_STOP			's'
#define CMD_MOTOR_GETSTEP		'g'

#endif // __USER_PROTOCOL_H__

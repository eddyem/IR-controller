/*
 * user_proto.h
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

#pragma once
#ifndef __USER_PROTO_H__
#define __USER_PROTO_H__

#include "cdcacm.h"
#include "uart.h"

// shorthand for prnt
#define P(arg, s) prnt((uint8_t*)arg, s)
#define LP(arg)   prnt((uint8_t*)arg, lastsendfun)
// debug message - over USB
#ifdef EBUG
	#define DBG(a) do{if(mode == BYTE_MODE) prnt((uint8_t*)a, lastsendfun);}while(0)
#else
	#define DBG(a)
#endif

#define MSG(argb, argl) do{if(mode == BYTE_MODE) prnt((uint8_t*)argb, lastsendfun); \
			else if(mode == LINE_MODE) prnt((uint8_t*)argl, lastsendfun);}while(0)

#define BYTE_MSG(arg) do{if(mode == BYTE_MODE) prnt((uint8_t*)arg, lastsendfun);}while(0)

#define ERR(arg) do{if(mode == BYTE_MODE){prnt((uint8_t*)"Error! ",lastsendfun); \
					prnt((uint8_t*)arg, lastsendfun);}}while(0)

// function to send a byte
typedef void (*sendfun)(uint8_t);

// function to process entered integer value at end of input
//return 1 if there's no output in this function (for echo), 0 if there's output
typedef uint8_t (*intfun)(int32_t, sendfun);

typedef enum{
	 BYTE_MODE   // data entered byte by byte interactively
	,LINE_MODE   // line mode like '[ command parameters ]\n'
	,BINARY_MODE // reserved
} curmode_t;

extern curmode_t mode;
extern sendfun lastsendfun; // last active send function - to post "anonymous" replies

void prnt(uint8_t *wrd, sendfun s);
//void newline(sendfun s);
#define newline(s)   s('\n')

void print_int(int32_t N, sendfun s);

int parce_incoming_buf(char *buf, int len, sendfun s);

// void process_int(int32_t v, sendfun s);
uint8_t set_ADC_gain(int32_t v, sendfun s);
void print_ad_vals(sendfun s);
void print_int_ad_vals(sendfun s);
uint8_t stepper_proc(int32_t v, sendfun s);
uint8_t set_timr(int32_t v, sendfun s);
void print_hex(uint8_t *buff, uint8_t l, sendfun s);


/**
 * here is commands definition (what is outhern function should print message?)
 * in line mode commands are echoed with some usefull information
 * in case of error inside command arguments will be word "ERR"
 */
#define CMD_INTADC_VALUES			'A'
#define STR_INTADC_VALUES			"A"
#define CMD_STOP_ALL_MOTORS			'B'
#define STR_STOP_ALL_MOTORS			"B"
#define CMD_ENDSW_STATE				'E'
#define STR_ENDSW_STATE				"E"
#define CMD_SHOW_PERIOD				'G'
#define STR_SHOW_PERIOD				"G"
#define CMD_SHTR_VOLTAGE			'h'
#define STR_SHTR_VOLTAGE			"h"
#define CMD_EXTADC_INIT				'i'
#define STR_EXTADC_INIT				"i"
#define CMD_MOTORS_VOLTAGE			'p'
#define STR_MOTORS_VOLTAGE			"p"
#define CMD_INIT_STATUS				'q'
#define STR_INIT_STATUS				"q"
#define CMD_EXTADC_VALUES			's'
#define STR_EXTADC_VALUES			"s"
#define CMD_SHTR_STATE				't'
#define STR_SHTR_STATE				"t"
#define CMD_PRINT_TIME				'T'
#define STR_PRINT_TIME				"T"
#define CMD_MOTOR_POSITION			'Z'
#define STR_MOTOR_POSITION			"Z"

#endif // __USER_PROTO_H__

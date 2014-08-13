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
// debug message - over USB
#ifdef EBUG
	#define DBG(a) prnt((uint8_t*)a, usb_send)
#else
	#define DBG(a)
#endif

#define MSG(arg) prnt((uint8_t*)arg, lastsendfun)

typedef void (*sendfun)(uint8_t); // function to send a byte
typedef void (*intfun)(int32_t, sendfun); // function to process entered integer value at end of input

extern sendfun lastsendfun; // last active send function - to post "anonymous" replies

void prnt(uint8_t *wrd, sendfun s);
//void newline(sendfun s);
#define newline(s)   s('\n')

void print_int(int32_t N, sendfun s);

void parce_incoming_buf(char *buf, int len, sendfun s);

void process_int(int32_t v, sendfun s);
void set_ADC_gain(int32_t v, sendfun s);
void print_ad_vals(sendfun s);

#endif // __USER_PROTO_H__
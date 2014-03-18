/*
 * main.h
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
#ifndef __MAIN_H__
#define __MAIN_H__
#include "stm32f10x.h"

extern volatile uint8_t MoreMessages;
// shorthand for prnt
#define P(arg) prnt((uint8_t*)arg)
// arg1 will be printed in "more messages" mode, else - arg2
#define MSG(arg1, arg2)  do{if(MoreMessages)P(arg1);else P(arg2);}while(0)

inline void Set_System();
inline void getTemp();
inline void getCode();
inline void prntADC();
inline void switch_Nth();
inline void ADG_off();
inline void prntSPD();
void findDev();
void printDev();

void prnt(uint8_t *wrd);
void newline();
void printInt(uint8_t *val, int8_t len);

void Delay(uint16_t time);
void Delay_SysTickHandler();


#endif // __MAIN_H__

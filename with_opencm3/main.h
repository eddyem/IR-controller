/*
 * main.h
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
#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <string.h>					// memcpy
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/spi.h>

#include "user_proto.h"
#include "AD7794.h"

#define _U_    __attribute__((__unused__))
#define  U8(x)  ((uint8_t)  x)
#define U16(x)  ((uint16_t) x)
#define U32(x)  ((uint32_t) x)

extern uint32_t ad7794_values[]; // array with ADC data
extern uint8_t doubleconv; // single/double ADC conversion
extern uint32_t ad7794_on; // ==1 after AD7794 initialisation
extern uint8_t ADC_monitoring; // ==1 to make continuous monitoring
void AD7794_init();

extern volatile uint32_t Timer; // global timer (milliseconds)
void Delay(uint16_t time);

void print_time(sendfun s); // print current time in milliseconds: 4 bytes for ovrvlow + 4 bytes for time

#endif // __MAIN_H__


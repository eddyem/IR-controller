/*
 * hardware_ini.h
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
#ifndef __HARDWARE_INI_H__
#define __HARDWARE_INI_H__

extern volatile uint16_t ADC_value[]; // ADC DMA value

#define ADC_ADDR_MASK	(GPIO6|GPIO7|GPIO8|GPIO9)
#define TRD_NO			8 // number of TRD devices

void GPIO_init();
void SysTick_init();
void ADC_init();
void ADC_calibrate_and_start();

#endif // __HARDWARE_INI_H__

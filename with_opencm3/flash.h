/*
 * flash.h
 *
 * Copyright 2015 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
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
#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"
#include "user_proto.h"

/*
 * this is a default values of stored data
 * they could be changed by appropriate command
 * align by 2k & make size 2k for using with high density devices
 */
#define FLASH_BLOCK_SIZE   (2048)
#define FLASH_WRONG_DATA_WRITTEN ((uint8_t)0x80)

#define FLASH_MAGICK ((uint32_t) 0xAA55A55A)

typedef struct{
	//uint32_t magick; // magick value
	// A-D value[x] = ADU * ADC_multipliers[x] / ADC_divisors[x]
	uint32_t _ADC_multipliers[ADC_CHANNELS_NUMBER];
	uint32_t _ADC_divisors[ADC_CHANNELS_NUMBER];
//	char last_addr[0]; // we need this pointer to calculate real size of structure
}stored_data;

typedef struct{
	stored_data all_stored;
	char struct_end[0] __attribute__ ((aligned(FLASH_BLOCK_SIZE))); // this pointer provides size of structure multiple of page size
} flash_data;

extern stored_data Stored_Data;

#define ADC_multipliers  Stored_Data._ADC_multipliers
#define ADC_divisors     Stored_Data._ADC_divisors

void dump_flash_data(sendfun s);
uint8_t save_flashdata();
void read_stored_data();

#endif // __FLASH_H__

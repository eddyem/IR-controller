/*
 * flash.c - functions to work with STM32 flash memory
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

#include "flash.h"
#include <libopencm3/stm32/flash.h>
#include <string.h>

const flash_data Stored_Data __attribute__ ((aligned(FLASH_BLOCK_SIZE))) = {
	//.magick = FLASH_MAGICK,
	._ADC_multipliers = {100000,100000,100000,100000,100000,100000,100000,100000, // TRD
		26, // shutter
		2   // power
	},
	._ADC_divisors = {1,1,1,1,1,1,1,1, // TRD
		25, // shutter
		7   // power
	}
};

uint32_t flash_write_data(uint32_t *dataptr, uint16_t datalen){
	uint32_t start_address = (uint32_t)&Stored_Data;
	uint16_t i, rem;
	uint32_t ret = 0;
	flash_unlock();
	DBG("erase\n");
	//Erasing page
	flash_erase_page(start_address);
	rem = datalen % 4; // remainder
	datalen /= 4;      // round to 4
	// copy data by blocks of four
	for(i = 0; i < datalen; i++, dataptr++, start_address += 4){
		// write data word
		flash_program_word(start_address, *dataptr);
		//verify
		if(*((uint32_t*)start_address) != *dataptr){
			ret = FLASH_WRONG_DATA_WRITTEN;
			goto endoffunction;
		}
	}
	// remainder
	if(rem){
		uint16_t halfwords[2] = {0,0}, n = 1;
		if(rem == 1) halfwords[0] = *((uint8_t*)dataptr);
		else halfwords[0] = *((uint16_t*)dataptr);
		if(rem == 3){ halfwords[1] = *((uint8_t*)dataptr+3); n = 2;}
		for(i = 0; i < n; i++, start_address += 2){
			flash_program_half_word(start_address, halfwords[i]);
			//verify
			if(*((uint16_t*)start_address) != halfwords[i]){
				ret = FLASH_WRONG_DATA_WRITTEN;
				goto endoffunction;
			}
		}
	}
	DBG("ok written\n");
endoffunction:
	flash_lock();
	return ret;
}

uint32_t  flash_store_U32(uint32_t addr, uint32_t *data){
	flash_data Saved_Data;
	uint32_t sz, ptrdiff;
	sz = (uint32_t)&Stored_Data.last_addr - (uint32_t)&Stored_Data;
	ptrdiff = addr - (uint32_t)&Stored_Data;
	memcpy((void*)&Saved_Data, (void*)&Stored_Data, sz);
	memcpy((void*)((uint32_t)&Saved_Data + ptrdiff), (void*)data, 4);
	return flash_write_data((uint32_t*)&Saved_Data, sz);
}

/**
 * printout all data stored in flash
 */
void dump_flash_data(sendfun s){
	int i;
//	P("magick: ", s);
//	print_int(Stored_Data.magick, s);
	P("\nADC multipliers: ", s);
	for(i = 0; i < ADC_CHANNELS_NUMBER; i++){
		if(i) P(", ", s);
		print_int(ADC_multipliers[i], s);
	}
	P("\nADC divisors: ", s);
	for(i = 0; i < ADC_CHANNELS_NUMBER; i++){
		if(i) P(", ", s);
		print_int(ADC_divisors[i], s);
	}
	s('\n');
}

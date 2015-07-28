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

// this is variable structure saved in RAM for ability of data changing
stored_data Stored_Data;

// this is constant structure saved in flash. It have to be copied to Stored_Data after run
const flash_data Flash_Data __attribute__ ((aligned(FLASH_BLOCK_SIZE))) = {
.all_stored = {
	//.magick = FLASH_MAGICK,
	._ADC_multipliers = {100000,100000,100000,100000,100000,100000,100000,100000, // TRD
		26, // shutter
		2   // power
	},
	._ADC_divisors = {1,1,1,1,1,1,1,1, // TRD
		25, // shutter
		7   // power
	},
	._OW_id_array = { // ID of 1-wire sensors
		{0x10, 0xad, 0xbc, 0x8f, 0x02, 0x08, 0x00, 0xf9},
		{0x10, 0x78, 0xe4, 0x8f, 0x02, 0x08, 0x00, 0x7d},
		{0x10, 0x46, 0x0a, 0x90, 0x02, 0x08, 0x00, 0x59},
		{0x10, 0x68, 0xd6, 0x8f, 0x02, 0x08, 0x00, 0x21},
		{0x10, 0x7c, 0xee, 0x8f, 0x02, 0x08, 0x00, 0x1c},
		{0x10, 0xf7, 0x02, 0x90, 0x02, 0x08, 0x00, 0x57},
		{0x10, 0x92, 0xf1, 0x8f, 0x02, 0x08, 0x00, 0x35},
		{0x10, 0x5f, 0x02, 0x90, 0x02, 0x08, 0x00, 0xaa}
	},
	._OW_dev_amount = 8
}
};

uint8_t flash_write_data(uint32_t *dataptr, uint16_t datalen){
	uint32_t start_address = (uint32_t)&(Flash_Data.all_stored);
	uint16_t i, rem;
	uint8_t ret = 0;
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

/**
 * save all data from RAM to flash
 */
uint8_t save_flashdata(){
//	uint32_t sz = (uint32_t)&Stored_Data.last_addr - (uint32_t)&Stored_Data;
	//return flash_write_data((uint32_t*)&Stored_Data, sz);
	return flash_write_data((uint32_t*)&Stored_Data, sizeof(stored_data));
}

void read_stored_data(){
//	uint32_t sz = (uint32_t)&Stored_Data.last_addr - (uint32_t)&Stored_Data;
	memcpy((void*)&Stored_Data, (void*)&(Flash_Data.all_stored), sizeof(stored_data));
}

/**
 * printout all data stored in flash
 */
void dump_flash_data(sendfun s){
	int i;
//	P("magick: ", s);
//	print_int(Stored_Data.magick, s);
	P("\nADC multipliers: ", s);
	for(i = 0; i < ADC_CHANNELS_NUMBER; ++i){
		if(i) P(", ", s);
		print_int(Flash_Data.all_stored._ADC_multipliers[i], s);
	}
	P("\nADC divisors: ", s);
	for(i = 0; i < ADC_CHANNELS_NUMBER; ++i){
		if(i) P(", ", s);
		print_int(Flash_Data.all_stored._ADC_divisors[i], s);
	}
	P("\n1-wire IDs:\n", s);
	for(i = 0; i < OW_dev_amount; ++i){
		int j;
		uint8_t *ROM = OW_id_array[i];
		s('\t');
		print_int(i, s);
		P(") ", s);
		for(j = 0; j < 8; ++j){
			if(j) P(", ", s);
			print_hex(&ROM[j], 1, s);
		}
		s('\n');
	}
}


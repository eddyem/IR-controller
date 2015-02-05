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

/*
 * this is a default values of stored data
 * they could be changed by appropriate command
 * align by 2k & make size 2k for using with high density devices
 */
#define FLASH_BLOCK_SIZE   (2048)
#define FLASH_WRONG_DATA_WRITTEN 0x80

const uint8_t _flash_buffer[FLASH_BLOCK_SIZE] __attribute__ ((aligned(FLASH_BLOCK_SIZE)));

/**
 *  these are default values
 * they can be changed in runtime to change data stored in flash
 */
flash_data Default_stored_data = {
	.magick = FLASH_MAGICK,
	._ADC_multipliers = {100000,100000,100000,100000,100000,100000,100000,100000, // TRD
		26, // shutter
		2   // power
	},
	._ADC_divisors = {1,1,1,1,1,1,1,1, // TRD
		25, // shutter
		7   // power
	}
};

flash_data *Stored_Data = (flash_data*) _flash_buffer;

uint32_t flash_write_data(uint8_t *var, uint8_t *data, uint16_t datalen){
	uint32_t start_address = (uint32_t)var, page_address = (uint32_t)Stored_Data;
	uint32_t *dataptr = (uint32_t*)data;
	uint16_t i, rem;
	uint32_t ret = 0;
	// check start address - it should be inside
	if((start_address - page_address) >= FLASH_BLOCK_SIZE){
//		DBG("bad starting address\n");
		return 1;
	}

	flash_unlock();
	//Erasing page
	flash_erase_page(page_address);
//DBG("erase flash ");
/*	if(FLASH_SR_EOP != (ret = flash_get_status_flags()))
		goto endoffunction;
*/
//DBG("OK\nwrite");
	rem = datalen % 4; // remainder
	datalen /= 4;      // round to 4
/*
print_int(datalen, lastsendfun);
DBG(" blocks of 4 bytes\n");
*/
	// copy data by blocks of four
	for(i = 0; i < datalen; i++, dataptr++, start_address += 4){
		// write data word
		flash_program_word(start_address, *dataptr);
/*		if(FLASH_SR_EOP != (ret = flash_get_status_flags()))
			goto endoffunction;
*/		//verify
		if(*((uint32_t*)start_address) != *dataptr){
			ret = FLASH_WRONG_DATA_WRITTEN;
			goto endoffunction;
		}
/*
DBG("Written: ");
print_int((int32_t)(uint32_t*)dataptr, lastsendfun);
lastsendfun('\n');
*/
	}
	// remainder
	if(rem){
		uint16_t halfwords[2] = {0,0}, n = 1;
		if(rem == 1) halfwords[0] = *((uint8_t*)dataptr);
		else halfwords[0] = *((uint16_t*)dataptr);
		if(rem == 3){ halfwords[1] = *((uint8_t*)dataptr+3); n = 2;}
		for(i = 0; i < n; i++, start_address += 2){
			flash_program_half_word(start_address, halfwords[i]);
			if(FLASH_SR_EOP != (ret = flash_get_status_flags()))
				goto endoffunction;
			//verify
			if(*((uint16_t*)start_address) != halfwords[i]){
				ret = FLASH_WRONG_DATA_WRITTEN;
				goto endoffunction;
			}
		}
	}
endoffunction:
	flash_lock();
/*
DBG("end, status: ");
print_int(ret, lastsendfun);
lastsendfun('\n');
*/
	return ret;
}


/**
 * checks magick in start of data block and fill block with default data
 * if flash is uninitialized
 */
uint32_t check_flash_data(){
	if(Stored_Data->magick == FLASH_MAGICK) return 0;
	DBG("copy data\n");
	return flash_write_data((uint8_t*)Stored_Data, (uint8_t*)&Default_stored_data, sizeof(flash_data));
}

/**
 * printout all data stored in flash
 */
void dump_flash_data(sendfun s){
	int i;
	P("magick: ", s);
	print_int(Stored_Data->magick, s);
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

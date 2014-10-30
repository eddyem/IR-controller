/*
 * spi.h
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
#ifndef __SPI_H__
#define __SPI_H__

#define SPI_BUFFERSIZE 4

typedef enum{
	SPI_NO_DATA,
	SPI_DATA_READY,
	SPI_READ_ERROR
} SPI_read_status;

void SPI_init();
uint8_t write_SPI(uint8_t *data, uint8_t len);
SPI_read_status check_SPI();
uint8_t *read_SPI(uint8_t *data, uint8_t len);

void switch_SPI(uint32_t SPI);

#endif // __SPI_H__

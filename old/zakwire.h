/*
 * zakwire.h
 *
 * Copyright 2013 Edward V. Emelianoff <eddy@sao.ru>
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
#ifndef __ZAKWIRE_H__
#define __ZAKWIRE_H__
#include "stm32f10x.h"

extern uint8_t ZW_status;
extern uint16_t ZW_result;

// ZW status
enum{
	ZW_OFF,
	ZW_START,
	ZW_PROCESS,
	ZW_DATARDY,
	ZW_ERR
};

void ZW_start_measurement();
void ZW_nodata();
void ZW_fill_next_bit(int32_t duty);
#endif // __ZAKWIRE_H__

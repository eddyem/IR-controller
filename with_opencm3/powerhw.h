/*
 * powerhw.h
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
#ifndef __POWERHW_H__
#define __POWERHW_H__

#include "hardware_ini.h"

// Shutter finite-state machine states
typedef enum{
	SHUTTER_NOTREADY = 0, // not ready, or error in work
	SHUTTER_INITIALIZED,  // initialisation in process
	SHUTTER_READY,        // ready for operation
	SHUTTER_OPENED,       // shutter is in opened state
	SHUTTER_CLOSED,       // shutter is in closed state
	SHUTTER_OPENING,      // user is waiting for opening
	SHUTTER_CLOSING,      // user is waiting for closing
	SHUTTER_PROC_OPENING, // shuter in work - waits for opening
	SHUTTER_PROC_CLOSING  // shuter in work - waits for closing
} shutter_state;

extern shutter_state Shutter_State;
extern uint16_t Shutter_delay;

shutter_state shutter_init();
void process_shutter();
void print_shutter_state(sendfun s);

uint8_t shutter_try(shutter_state state);

#define try_to_close_shutter()   shutter_try(SHUTTER_CLOSING)
#define try_to_open_shutter()    shutter_try(SHUTTER_OPENING)

#endif // __POWERHW_H__

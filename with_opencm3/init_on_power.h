/*
 * init_on_power.h
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
#ifndef __INIT_ON_POWER_H__
#define __INIT_ON_POWER_H__

#include "user_proto.h"

// states of system-on-poweron initialisation
typedef enum{
	 INI_START
	,INI_FOCUS_PROC
	,INI_SIN_PROC
	,INI_READY
} started_ini;

extern started_ini stini_status;

void init_on_poweron_proc();
void print_init_status();
#define forced_init()  do{stini_status = INI_START;}while(0)

#endif // __INIT_ON_POWER_H__

/*
 * init_on_power.c
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

#include "main.h"
#include "init_on_power.h"
#include "stepper_motors.h"

started_ini stini_status = INI_START;

void init_on_poweron_proc(){
	switch (stini_status){
		case INI_START:
			move_motor(4, -4500); // move focus to zero
			stini_status = INI_FOCUS_PROC;
		break;
		case INI_FOCUS_PROC:
			if(Motor_active[4]) return; // focus is still moving
			move_motor(3, -55000); // move sinus mechanism to zero
			stini_status = INI_SIN_PROC;
		break;
		case INI_SIN_PROC:
			if(Motor_active[3]) return;
			stini_status = INI_READY;
		break;
		case INI_READY:
		default:
			return;
	}
}

void print_init_status(){
	MSG("ini status: ", "[ " STR_INIT_STATUS " ");
	switch (stini_status){
		case INI_START:
			LP("started");
		break;
		case INI_FOCUS_PROC:
			LP("focus");
		break;
		case INI_SIN_PROC:
			LP("sin");
		break;
		case INI_READY:
		default:
			LP("ready");
	}
	if(mode == LINE_MODE) LP(" ]");
	lastsendfun('\n');
}

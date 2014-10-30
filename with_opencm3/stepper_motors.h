/*
 * stepper_motors.h
 *
 * Copyright 2014 Edward V. Emelianoff <eddy@sao.ru>
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
#ifndef __STEPPER_MOTORS_H__
#define __STEPPER_MOTORS_H__

#include <libopencm3/stm32/timer.h>

void steppers_init();
void process_stepper_motors();
void move_motor(uint8_t num, int32_t steps);
void stop_motor(uint8_t num);
void set_motor_period(uint8_t num, uint16_t period);

#endif // __STEPPER_MOTORS_H__

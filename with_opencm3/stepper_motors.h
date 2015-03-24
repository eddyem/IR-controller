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

// default pause to make sure that turret is on position
#define TURRETS_PAUSE_US 30000
// max amount of steps to add to turret for moving to next position
#define TURRETS_NEXT_POS_STEPS  300

#ifndef CONCAT
#define CONCAT(A, B) A ## B
#endif

// check status of end-switches and Halls
// Motor 0 == turret 0, PD0..PD3
#define _CHECK_EP0  ((~GPIO_IDR(GPIOD)) & 0x0f)
// Motor 1 == turret 1, PD4..PD6
#define _CHECK_EP1  (((~GPIO_IDR(GPIOD)) >> 4) & 0x07)
// Motor 2 == turret 2, PD7, PB6, PB7
#define _CHECK_EP2  ((((~GPIO_IDR(GPIOD)) >> 7) & 0x01) | (((~GPIO_IDR(GPIOB))>> 6) & 0x03))
// Motor 3 == long (VPHG, pupil stop) stage, PC7/PC8  (down/up)
#define _CHECK_EP3  (((~GPIO_IDR(GPIOC)) >> 7) & 0x03)
// Motor 4 == short (focus) stage, PC9/PA8 (down/up)
#define _CHECK_EP4  ((((~GPIO_IDR(GPIOC)) >> 9) & 0x01) | (((~GPIO_IDR(GPIOA)) >> 8) & 0x01))
// this macro returns end-switches & Hall status: 0 - not active, 1 - active
#define CHECK_EP(X)   CONCAT(_CHECK_EP, X)
// end-switches for motors 3,4 (stage 1 and stage 2): stop when direction positive/negative
#define EP3PLUS     2
#define EP3MINUS    1
#define EP4PLUS     2
#define EP4MINUS    1
#define STAGE_CHECK(N, DIR)  CONCAT(EP ## N, DIR)

// setup ports: PA8, PB6, PB7, PC7..PC9, PD0..PD7
/*
#define SETUP_ESW() do{gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO8); \
                       gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO6|GPIO7); \
                       gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, (uint16_t)0x0380);\
                       gpio_set_mode(GPIOD, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, (uint16_t)0xff); \
                    }while(0)
*/
#define SETUP_ESW() do{gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO8); \
                       gpio_set(GPIOA, GPIO8); \
                       gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO6|GPIO7); \
                       gpio_set(GPIOB, GPIO6|GPIO7); \
                       gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, (uint16_t)0x0380);\
                       gpio_set(GPIOC, (uint16_t)0x0380);\
                       gpio_set_mode(GPIOD, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, (uint16_t)0xff); \
                       gpio_set(GPIOD, (uint16_t)0xff); \
                    }while(0)

extern uint8_t move2pos[]; // export this array for ability of moving turret to given position from outside

void steppers_init();
void process_stepper_motors();
uint8_t move_motor(uint8_t num, int32_t steps);
void stop_motor(uint8_t num);
void set_motor_period(uint8_t num, uint16_t period);
uint8_t check_ep(uint8_t num);


#endif // __STEPPER_MOTORS_H__

/*
 * leds.h
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
#ifndef __LEDS_H__
#define __LEDS_H__

#include "stm32f10x.h"

#define LEDSTATE_UNKNOWN		0x00
#define LEDSTATE_OFF			0x01
#define LEDSTATE_ON				0x10
#define LEDSTATE_BLINK			0x20

uint8_t LED_GetState();
uint8_t LED_GetBlinkState(uint16_t *blink_on, uint16_t *blink_off);
void LED_On();
void LED_Off();
void LED_OnBlink();
void LED_Duty(uint8_t duty); // set duty cycle
void LED_DutyPlus(); // increase duty cycle
void LED_DutyMinus();// decrease duty cycle

void LED_SysTick_Handler();

#endif // __LEDS_H__

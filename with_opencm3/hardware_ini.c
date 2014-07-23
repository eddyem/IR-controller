/*
 * hardware_ini.c - functions for HW initialisation
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

#include "main.h"
#include "hardware_ini.h"

/**
 * GPIO initialisaion: clocking + ports setup
 */
void GPIO_init(){
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, GPIO11|GPIO12); // LED + USB
}

void SysTick_init(){
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // Systyck: 72/8=9MHz
	systick_set_reload(8999); // 9000 pulses: 1kHz
	systick_interrupt_enable();
	systick_counter_enable();
}



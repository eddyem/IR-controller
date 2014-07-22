/*
 * main.c
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

uint32_t Timer = 0; // global timer (milliseconds)

int main(){
	int i;
	uint32_t Old_timer = 0;

	usbd_device *usbd_dev;
	//rcc_clock_setup_in_hsi_out_48mhz();
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	rcc_periph_clock_enable(RCC_GPIOC);

	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO11|GPIO12);
	gpio_set(GPIOC, GPIO11); // turn off USB
	gpio_clear(GPIOC, GPIO12); // turn on LED

	usbd_dev = USB_init();

	for (i = 0; i < 0x800000; i++)
		__asm__("nop");
	gpio_clear(GPIOC, GPIO11);

	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // Systyck: 72/8=9MHz
	systick_set_reload(8999); // 9000 pulses: 1kHz
	systick_interrupt_enable();
	systick_counter_enable();

	while(1){
		usbd_poll(usbd_dev);
		if(Timer - Old_timer > 1000){ // write out time in seconds
			Old_timer += 1000;
			gpio_toggle(GPIOC, GPIO12); // toggle LED
			//print_int(Timer/1000, usb_send);
		}
	}
}

void sys_tick_handler(){
	usb_send_buffer();
	Timer++;
}

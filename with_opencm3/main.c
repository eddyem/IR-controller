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
#include "hardware_ini.h"
#include "cdcacm.h"
#include "uart.h"

uint32_t Timer = 0; // global timer (milliseconds)

int main(){
	int i;
	uint32_t Old_timer = 0;

	usbd_device *usbd_dev;
	//rcc_clock_setup_in_hsi_out_48mhz();
	// RCC clocking: 8MHz oscillator -> 72MHz system
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	// GPIO
	GPIO_init();
	gpio_set(GPIOC, GPIO11); // turn off USB
	gpio_clear(GPIOC, GPIO12); // turn on LED

	// init USART1
	UART_init(USART1);

	// USB
	usbd_dev = USB_init();

	// wait a little and then turn on USB pullup
	for (i = 0; i < 0x800000; i++)
		__asm__("nop");
	gpio_clear(GPIOC, GPIO11);

	// SysTick is a system timer with 1mc period
	SysTick_init();

	while(1){
		usbd_poll(usbd_dev);
		if(Timer - Old_timer > 1000){ // write out time in seconds
			Old_timer += 1000;
			gpio_toggle(GPIOC, GPIO12); // toggle LED
			//print_int(Timer/1000, usb_send);
		}
	}
}

/**
 * SysTick interrupt: increment global time & send data buffer through USB
 */
void sys_tick_handler(){
	usb_send_buffer();
	Timer++;
}

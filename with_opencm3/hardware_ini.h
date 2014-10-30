/*
 * hardware_ini.h
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

#pragma once
#ifndef __HARDWARE_INI_H__
#define __HARDWARE_INI_H__



extern volatile uint16_t ADC_value[]; // ADC DMA value

#define TRD_NO			8 // number of TRD devices
// bits used to address external SPI ADC - PD10..12
#define ADC_ADDR_MASK	(GPIO10|GPIO11|GPIO12)
#define ADC_SET_ADDR(X)	((X << 10) & ADC_ADDR_MASK)
#define ADC_EN_PIN		GPIO13
#define ADC_ADDR_PORT	GPIOD

void SPI1_init();
void SPI2_init();
void GPIO_init();
void SysTick_init();
void ADC_init();
void ADC_calibrate_and_start();

/*
 * USB interface
 */
// USB_DICS (disconnect) - PC11
#define USB_DISC_PIN		GPIO11
#define USB_DISC_PORT		GPIOC
// USB_POWER (high level when USB connected to PC)
#define USB_POWER_PIN		GPIO10
#define USB_POWER_PORT		GPIOC
// change signal level on USB diconnect pin
#define usb_disc_high()   gpio_set(USB_DISC_PORT, USB_DISC_PIN)
#define usb_disc_low()    gpio_clear(USB_DISC_PORT, USB_DISC_PIN)
// in case of pnp bipolar transistor on 1.5k pull-up disconnect means low level
// in case of p-channel FET on 1.5k pull-up change on/off disconnect means high level
#define usb_disconnect()  usb_disc_low()
#define usb_connect()     usb_disc_high()

/*
 * Stepper motors
 * motors' interface use timers 1 & 2, so even hardware-dependent functions are
 * 		moved to stepper_motors.c
 */
// PE7..11 - EN
#define MOROT_EN_MASK		(0x1f << 7)
#define MOTOR_EN_PORT		(GPIOE)
// N == 1..5
#define MOTOR_EN_PIN(N)		(GPIO6 << (N))
// PE0..PE5 - DIR
#define MOROT_DIR_MASK		(0x1f)
#define MOTOR_DIR_PORT		(GPIOE)
// N == 1..5
#define MOTOR_DIR_PIN(N)	(GPIO0 << (N - 1))
// timers: TIM1 - PC6, TIM2 - PD15
#define MOTOR_TIM1_PORT		(GPIOC)
#define MOTOR_TIM1_PIN		(GPIO6)
#define MOTOR_TIM2_PORT		(GPIOD)
#define MOTOR_TIM2_PIN		(GPIO15)

/*
 * One Wire interface
 */
// In case of using other USART for 1-wire port, make corresponding change
// and redefine pins in OW_Init
#define OW_USART_X			USART2
#define OW_RX_PORT			GPIO_BANK_USART2_RX
#define OW_RX_PIN			GPIO_USART2_RX

void init_dmatimer();
void run_dmatimer();

#endif // __HARDWARE_INI_H__

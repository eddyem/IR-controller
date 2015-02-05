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

/*
 * Timers:
 * SysTick - system time
 * TIM1 - not used
 * TIM2 - 1-wire
 * TIM3, TIM4 - stepper motors
 * TIM5 - pause for shutter
 */


#define TIM2_DMABUFF_SIZE 128
// 1-wire zero-state lengths (in us minus 1)
#define OW_1           (9)
#define OW_0           (69)
#define OW_READ1       (14)
#define OW_BIT         (79)
#define OW_RESET       (499)
#define OW_RESET_TIME  (999)
#define OW_PRESENT     (549)

extern volatile uint16_t ADC_value[]; // ADC DMA value

#define TRD_NO			(8) // number of TRD devices
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
// in case of n-channel FET on 1.5k pull-up change on/off disconnect means low level
// in case of pnp bipolar transistor or p-channel FET on 1.5k pull-up disconnect means high level
#define usb_disconnect()  usb_disc_high()
#define usb_connect()     usb_disc_low()

/*
 * Stepper motors
 * motors' interface use timers 1 & 2, so even hardware-dependent functions are
 * 		moved to stepper_motors.c
 */
// PE7..11 - EN
#define MOTOR_EN_MASK		(0x1f << 7)
#define MOTOR_EN_PORT		(GPIOE)
// N == 0..4
#define MOTOR_EN_PIN(N)		(GPIO7 << (N))
// PE0..PE5 - DIR
#define MOTOR_DIR_MASK		(0x1f)
#define MOTOR_DIR_PORT		(GPIOE)
// N == 0..4
#define MOTOR_DIR_PIN(N)	(GPIO0 << (N))
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

/*
 * Shutter defines
 */
// We use timer 5 to process pauses with shutter
#define Shutter_tim_isr   tim5_isr
#define SHUTTER_TIM       TIM5
#define NVIC_SHUTTER_IRQ  NVIC_TIM5_IRQ
// Shutter pins: PC0 & PC2 are polarity & on/off pins; PC1 is feedback pin
#define SHUTTER_PORT           (GPIOC)
#define SHUTTER_ON_PIN         (GPIO2)
#define SHUTTER_POLARITY_PIN   (GPIO0)
#define SHUTTER_FB_PIN         (GPIO1)
// voltage (*100) threshold to run shutter - 20V
#define SHUTTER_VOLTAGE_THRES  (2000)
// minimum voltage that should be on capasitor if power source is on
#define SHUTTER_UNDERVOLTAGE_THRES  (700)
// delay in operations (open/close) in us (according to shutter's datasheet it's about 12ms)
#define SHUTTER_DELAY  (12500)
// delay for error test
#define SHUTTER_OP_DELAY (200)

// ADC_value[8] is U36, ADC_value[9] is U10
#define SHUTTER_SENSE_NUMBER  (8)
#define POWER_SENSE_NUMBER    (9)
int shutter_voltage();
int power_voltage();
int TRD_value(uint8_t num);

void init_ow_dmatimer();
void run_dmatimer();
extern uint8_t ow_done;
#define OW_READY()  (ow_done)
void ow_dma_on();
void adc_dma_on();
uint8_t OW_add_byte(uint8_t ow_byte, uint8_t Nbits, uint8_t ini);
uint8_t OW_add_read_seq(uint8_t Nbytes);
void read_from_OWbuf(uint8_t start_idx, uint8_t N, uint8_t *outbuf);
void ow_reset();
uint8_t OW_get_reset_status();
#endif // __HARDWARE_INI_H__

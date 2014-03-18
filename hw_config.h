/*
 * hw_config.h - some definitions of hardware configuration, be careful:
 *      interrupts.c also have hardware-dependent parts!
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
#ifndef __HW_CONFIG_H__
#define __HW_CONFIG_H__

#include "stm32f10x.h"
/*
 * here nailed next config pins:
 *
 * USB disconnect:		PC11
 * User button:			PA0
 * LED:					PC12
 * Hall Sensor:			PD2
 * ADC:					PB0
 * ADG506 control:		PC0..4
 * 1-wire:				PB10,11
 * stepper motors:		PA8 (Timer1cc1), PB1, PC6..PC10
 * ZacWire:				PA1 (TIM2_CH2), PB8 (ZW power)
 */
#define ADC1_DR_Address    ((uint32_t)0x4001244C)

/*
 * Hall sensor
 */
// Hall sensor GPIO port
#define Hall_GPIOx			GPIOD
// Hall sensor pin
#define Hall_PIN			GPIO_Pin_2


/*
 * One Wire interface
 */
// In case of using USART2 for 1-wire port, make corresponding change
// and redefine pins in OW_Init
//#define OW_USART_X			USART2
#define OW_USART_X			USART3
// USART2 is on DMA channels 6(RX) & 7(TX); USART3 is on channels 2 & 3
//#define OW_DMA_TX_CHANNEL	DMA1_Channel7
#define OW_DMA_TX_CHANNEL	DMA1_Channel2
//#define OW_DMA_RX_CHANNEL	DMA1_Channel6
#define OW_DMA_RX_CHANNEL	DMA1_Channel3
// wait EndOfTransmission without interrupts, change DMA1_FLAG_TC6 to appropriate
// flags: GL-global interrupt, HT-half transfer, TC-transfer complete, TE-transfer error
//#define DMA_TRANSFER_END (DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET)
#define OW_DMA_TRANSFER_END (DMA_GetFlagStatus(DMA1_FLAG_TC3) == RESET)

/*
 * Motor interface
 */
// Timer to PWG generation
#define SM_Timer			TIM1
// compare-capture channel of motor timer
#define SM_IT_CC			(TIM_IT_CC1)
// CC reload register of motor timer
#define SM_CCR				CCR1
// PUL - step impulses
#define SM_PUL_GPIOx		GPIOA
#define SM_PUL_PIN			GPIO_Pin_8
// DIR - cw/ccw
#define SM_DIR_GPIOx		GPIOB
#define SM_DIR_PIN			GPIO_Pin_1
// EN - enable motor X
// !!! bits MUST BE CONSEQUENT !!!
#define SM_EN_GPIOx			GPIOC
// all enable pins
#define SM_EN_PINS			((uint16_t)0x7c0)
// motor0 - all other are SM_EN0_PIN << (motor number)
#define SM_EN0_PIN			GPIO_Pin_6

/*
 * ZacWire interface
 */
#define ZW_Timer			TIM2
// PB8 - ZacWire power
#define ZW_PWR_GPIOx		GPIOB
#define ZW_PWR_PIN			GPIO_Pin_8
// PA8 - timer input
#define ZW_DATA_GPIOx		GPIOA
#define ZW_DATA_PIN			GPIO_Pin_1
// ZW timer
#define ZW_TIM_CHNL			TIM_Channel_2
#define ZW_TRIGGER			TIM_TS_TI2FP2
// ZW interrupts. When changing TIM_IT_CC3 to something else, make also changes  in zakwire.c
#define ZW_INTRPTS			(TIM_IT_CC2 | TIM_IT_CC3)
// RCC
void RCC_Config(void);
// packet size for data to be sent to USB
#define USART_RX_DATA_SIZE   512

void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);

void Set_USBClock(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);

void Handle_USBAsynchXfer (void);
void USB_Send_Data(uint8_t data);

void Ports_Config(void);

extern __IO uint16_t ADC_value;
#endif // __HW_CONFIG_H__

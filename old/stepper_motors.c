/*
 * stepper_motors.c - moving of stepper motors
 *
 * Copyright 2013 Edward V. Emelianoff <eddy@sao.ru>
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
#include "includes.h"

uint8_t Motor_number = 0;// number of active motor
static uint8_t timers_activated = 0; // flag of activated timers
// one pulse period *1e-5s (one step == 16 pulses)
uint16_t Motor_period = 25; // near 250 steps per second
// amount of steps
volatile uint32_t Motor_steps = 0;

static void activate_timers(){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	/*
	 * Time base configuration
	 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = Motor_period;
	// timer prescaler: 10mks period
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 100000) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // no division
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(SM_Timer, &TIM_TimeBaseStructure);
	/*
	 * compare/toggle configuration
	 * Channel 1 will toggle its state every timer pulse
	 */
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Motor_period / 2; // instead of 50% we can do constant length impulses
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(SM_Timer, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(SM_Timer, TIM_OCPreload_Enable);
	// enable IRQs & run timer
	if(User_value < 0){
		User_value = -User_value;
		SM_DIR_GPIOx->BSRR = SM_DIR_PIN; // set DIR bit to rotate ccw
	}else
		SM_DIR_GPIOx->BRR = SM_DIR_PIN; // reset DIR bit
	Motor_steps = ((uint32_t)User_value) << 4; // multiply by 16 to get steps count
	//printInt((uint8_t*)&Motor_steps, 4);
	//newline();
	TIM_Cmd(SM_Timer, ENABLE);
	TIM_CtrlPWMOutputs(SM_Timer, ENABLE);
	// configure interrupts: only from CC1 (that's mean that all is over)
	TIM_ITConfig(SM_Timer, SM_IT_CC, ENABLE);
	timers_activated = 1;
}

/**
 * Move motor Motor_number to User_value steps
 */
void move_motor(){
	if(User_value == 0) return;
	// check whether motor is moving
	if(timers_activated){
		MSG("err: moving", "err");
		newline();
		return;
	}
	// activate motor # Motor_number
	SM_EN_GPIOx->BSRR = (SM_EN0_PIN << (Motor_number)); // set proper bit
	// set steps counter to User_value & run timer
	activate_timers();
}


/**
 * Stop timer1 when all steps are over; turn off motor voltage
 */
void stop_timer1(){
	// disable IRQs & stop timer
	TIM_Cmd(SM_Timer, DISABLE);
	TIM_ITConfig(SM_Timer, TIM_IT_Update, DISABLE);
	// turn off power
	SM_EN_GPIOx->BRR = SM_EN_PINS; // reset all EN bits
	SM_PUL_GPIOx->BRR = SM_PUL_PIN; // reset signal on PUL
	timers_activated = 0;
	Motor_steps = 0;
}

/**
 * Sets motor period to user value & refresh timer
 */
void set_motor_period(){
	Motor_period = (uint16_t)User_value;
	SM_Timer->ARR  = Motor_period;
	SM_Timer->SM_CCR = Motor_period / 2;
}

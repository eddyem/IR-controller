/*
 * zakwire.c - functions to work with TSic-506 temperature sensors
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

#include  "includes.h"

static uint8_t timers_activated = 0; // flag of activated timers
// status: turned off, processing, data ready or error
uint8_t ZW_status = ZW_OFF;
// result - value taken from TSic
uint16_t ZW_result;
// counter of current bit on ZW protocol
static uint8_t bit_cntr;


static void init_timers(){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	// timer prescaler: 1mks period, refresh every tick
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(ZW_Timer, &TIM_TimeBaseStructure);
	// timer input setup
	TIM_ICInitTypeDef TIM_ICStructure;
	TIM_ICStructure.TIM_Channel = ZW_TIM_CHNL; // select channel
	TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // interrupt on pulse end
	TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // take input direct from pin
	TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // no prescaler
	TIM_ICStructure.TIM_ICFilter = 0; // without filter
	// switch to PWM input
	TIM_PWMIConfig(ZW_Timer, &TIM_ICStructure);
	// select
	TIM_SelectInputTrigger(ZW_Timer, ZW_TRIGGER);
	// reset counter on trigger
	TIM_SelectSlaveMode(ZW_Timer, TIM_SlaveMode_Reset);
	// enable redirection of signal into other inputs
	TIM_SelectMasterSlaveMode(ZW_Timer, TIM_MasterSlaveMode_Enable);
	/*
	 * Third channel with timeout for 50ms used for errors detection
	 *   (no signal from ZW)
	 */
	TIM_OCInitTypeDef TIM_OCStructure;
	TIM_OCStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCStructure.TIM_Pulse = 50000;
	TIM_OC3Init(ZW_Timer, &TIM_OCStructure);
	// turn on timer
	TIM_Cmd(ZW_Timer, ENABLE);
	// Enable interrupts from ZW data & from error timer
	TIM_ITConfig(ZW_Timer, ZW_INTRPTS, ENABLE);
	timers_activated = 1;
}

/**
 * Turn off ZW timer & TSic power
 * make ZW_status = ZW_OFF in main.c
 */
void ZW_off(){
	//ZW_PWR_GPIOx->BRR = ZW_PWR_PIN; // reset pin to turn off power
	ZW_PWR_GPIOx->BSRR = ZW_PWR_PIN;
	TIM_Cmd(ZW_Timer, DISABLE);
	TIM_ITConfig(ZW_Timer, ZW_INTRPTS, DISABLE);
	timers_activated = 0;
}

/**
 * init timers & start measuring
 */
void ZW_start_measurement(){
	if(timers_activated){ // user tries to run function again
		MSG("err: measure", "err");
		newline();
		return;
	}
	// start timer & turn on TSic power
	//ZW_PWR_GPIOx->BSRR = ZW_PWR_PIN; // set pin to turn power on
	ZW_PWR_GPIOx->BRR = ZW_PWR_PIN;
	// zero all data
	bit_cntr = 0;
	ZW_result = 0;
	ZW_status = ZW_START;
	init_timers();
}

/**
 * check bit counter & duty value, fill next bit
 */
void ZW_fill_next_bit(int32_t duty){
//printInt(&bit_cntr, 1);
//printInt((uint8_t*)&duty, 1); newline();
	bit_cntr++; // now this is a number of *next* bit
	// check strobe bits & omit it
	if(bit_cntr == 1 || bit_cntr == 11){
		if(duty > 62 || duty < 38) // error: duty should be approx. equal to 50
			ZW_nodata(); // emergency stop
		return;
	}
	// omit CRC bit
	if(bit_cntr == 10) return;
	// process Nth bit & check duty value
	ZW_result <<= 1;
	if(duty < 30) ZW_result |= 1; // duty cycle > 70% (zero pulse < 30%) == 1
	else if(duty < 70) // error bit
		ZW_nodata();
	// if this is a last data bit - mark that all is done!
	if(bit_cntr == 19){
		ZW_off();
		ZW_status = ZW_DATARDY;
		return;
	}
}

/**
 * Error: no data for two periods
 */
void ZW_nodata(){
	ZW_off();
	ZW_status = ZW_ERR;
}

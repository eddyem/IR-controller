/*
 * powerhw.c - functions to work with other power loads (shutter, heaters and so on)
 *
 * Copyright 2015 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
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
// state of shutter - global variable to omit interface functions
shutter_state Shutter_State = SHUTTER_NOTREADY;
uint16_t Shutter_delay = SHUTTER_DELAY;

// function to be runned from timer irq
void (*shutter_timer_fn)() = NULL;

/**
 * Make background pause in 'us' microsecond, after which run function fn_ready
 * @param us       - pause in microseconds
 * @param fn_ready - function to run at end of pause
 */
void shutter_wait(int us, void(*fn_ready)()){
	if(!fn_ready) return;
	//DBG("wait for previous .. ");
	while(shutter_timer_fn); // wait for ending of previous operation
	//DBG("ok\n");
	shutter_timer_fn = fn_ready;
	timer_set_period(SHUTTER_TIM, us);
	timer_enable_counter(SHUTTER_TIM);
}

// macro to open/close/set default state
// open shutter is 0:0 -> when MCU power is off, shutter "automatically" opens
#define shutter_open()  do{gpio_clear(SHUTTER_PORT, SHUTTER_ON_PIN | SHUTTER_POLARITY_PIN);}while(0)
#define shutter_close() do{gpio_clear(SHUTTER_PORT, SHUTTER_ON_PIN);  \
		gpio_set(SHUTTER_PORT, SHUTTER_POLARITY_PIN);}while(0)
#define shutter_hiZ() do{gpio_set(SHUTTER_PORT, SHUTTER_ON_PIN | SHUTTER_POLARITY_PIN);}while(0)
#define shutter_off() do{gpio_set(SHUTTER_PORT, SHUTTER_ON_PIN); \
		gpio_clear(SHUTTER_PORT, SHUTTER_POLARITY_PIN);}while(0)
#define shutter_error() ((gpio_get(SHUTTER_PORT, SHUTTER_FB_PIN) == 0))

/**
 * Make tests for undervoltage & wire breakage
 * this function calls in 2 stages:
 *    1) check undervoltage & set hiZ
 *    2) repeat checking undervoltage & check wire breakage
 * if shutter was uninitialized we open it
 */
void shutter_test(){
	static shutter_state old_State = SHUTTER_NOTREADY;
	// test for undervoltage
	if(shutter_voltage() < SHUTTER_UNDERVOLTAGE_THRES){
		ERR("shutter undervoltage\n");
		Shutter_State = SHUTTER_NOTREADY;
		shutter_off();
		return;
	}
	if(Shutter_State != SHUTTER_INITIALIZED){ // first call of this function
		old_State = Shutter_State;
		Shutter_State = SHUTTER_INITIALIZED;
		// test for wire breakage
		DBG("breakage test\n");
		shutter_hiZ();  // 1,1: breakage test
		shutter_wait(SHUTTER_OP_DELAY, shutter_test);
	}else{ // check breakage
		if(shutter_error()){ // ERR==0 -> wire breakage
			ERR("shutter wire breakage\n");
			Shutter_State = SHUTTER_NOTREADY;
		}else{
			if(old_State == SHUTTER_NOTREADY){
				Shutter_State = SHUTTER_OPENING;
				DBG("ready!\n");
			}else{
				Shutter_State = old_State;
				DBG("no errors\n");
			}
		}
		shutter_off();
	}
}


/**
 * after open/close pulse we should return bridge to default state
 * 3 stages:
 *    1) check for short-circuit & turn off power of the shutter
 *    2) check for overtemperature or undervoltage
 *    3) common test for undervoltage/breakage
 */
void shutter_ready(){
	uint8_t test_err = 0;
	//DBG("shtr ready\n");
	switch (Shutter_State){
		case SHUTTER_CLOSED: // repeated pulse to check errors
		case SHUTTER_OPENED:
			if(shutter_error()){
				ERR("shutter overtemperature or undervoltage\n");
				Shutter_State = SHUTTER_NOTREADY;
			}
		break;
		case SHUTTER_PROC_CLOSING: // closing - set to closed state
		case SHUTTER_PROC_OPENING: // opening - set to opened state
			if(shutter_error()){
				ERR("shutter short-circuit\n");
				Shutter_State = SHUTTER_NOTREADY;
			}else{
				test_err = 1;
				if(Shutter_State == SHUTTER_PROC_CLOSING)
					Shutter_State = SHUTTER_CLOSED;
				if(Shutter_State == SHUTTER_PROC_OPENING)
					Shutter_State = SHUTTER_OPENED;
			}
		break;
		default:
			ERR("wrong shutter state\n");
			print_shutter_state(lastsendfun);
	}
	shutter_off();
	if(Shutter_State == SHUTTER_NOTREADY) return;
	if(test_err){
		//DBG("now test for err\n");
		shutter_wait(SHUTTER_OP_DELAY, shutter_ready); // test for overtemp or undervoltage
	}else{
		// wait a lot of time to prevent false detections
		shutter_wait(SHUTTER_DELAY, shutter_test);
	}
}

/**
 * Initialisation of shutter ports & test for shutter presense
 * @return 1 if all OK, 0 in case of error
 */
shutter_state shutter_init(){
	Shutter_State = SHUTTER_NOTREADY;
	// setup timer
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM5EN);// enable timer clocking
	nvic_enable_irq(NVIC_SHUTTER_IRQ);
	timer_reset(SHUTTER_TIM);
	// timer have frequency of 1MHz, so, to make pause in Xus set period to X
	// 36MHz of APB1
	timer_set_mode(SHUTTER_TIM, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	// 72MHz div 72 = 1MHz
	timer_set_prescaler(SHUTTER_TIM, 71);
	timer_one_shot_mode(SHUTTER_TIM);   // single pulse mode
	timer_disable_preload(SHUTTER_TIM); // force changing period
	timer_update_on_overflow(SHUTTER_TIM); // generate UEV only on overflow
	timer_enable_update_event(SHUTTER_TIM);
	timer_enable_irq(SHUTTER_TIM, TIM_DIER_UIE); // update IRQ enable
	//DBG("shutter timer ready\n");
	// setup pins
	// on/off & polarity: open drain
	gpio_set_mode(SHUTTER_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_OPENDRAIN, SHUTTER_ON_PIN | SHUTTER_POLARITY_PIN);
	// feedback: floating input
	gpio_set_mode(SHUTTER_PORT, GPIO_MODE_INPUT,
				GPIO_CNF_INPUT_FLOAT, SHUTTER_FB_PIN);
	shutter_off();
	shutter_wait(SHUTTER_OP_DELAY, shutter_test);
	return SHUTTER_INITIALIZED; // we return this state in spite of the shutter isn't really initialized yet
}

/**
 * Finite-state machine processing for shutter
 * (wait for capasitor charge and run needed functions
 */
void process_shutter(){
	if(Shutter_State == SHUTTER_NOTREADY) return;

	if(Shutter_State != SHUTTER_OPENING && Shutter_State != SHUTTER_CLOSING)
		return;
	if(shutter_voltage() < SHUTTER_UNDERVOLTAGE_THRES){
		ERR("no shutter power source\n");
		Shutter_State = SHUTTER_NOTREADY;
		shutter_off();
		return;
	}
	if(shutter_error()){
		ERR("some shutter error\n");
		Shutter_State = SHUTTER_NOTREADY;
		shutter_off();
		return;
	}
	if(shutter_voltage() < SHUTTER_VOLTAGE_THRES) return; // capasitor isn't charged
	switch (Shutter_State){
		case SHUTTER_OPENING:
			Shutter_State = SHUTTER_PROC_OPENING;
			shutter_open();
		break;
		case SHUTTER_CLOSING:
			Shutter_State = SHUTTER_PROC_CLOSING;
			shutter_close();
		break;
		default:
			return;
	}
	shutter_wait(Shutter_delay, shutter_ready);
}

void Shutter_tim_isr(){
	//DBG("shutter timer\n");
	void (*fn)() = shutter_timer_fn;
	if(timer_get_flag(SHUTTER_TIM, TIM_SR_UIF)){
		// Clear compare interrupt flag
		timer_clear_flag(SHUTTER_TIM, TIM_SR_UIF);
		shutter_timer_fn = NULL;
		// and run needed function
		if(fn){
			fn();
		}
	}
}

/**
 * printout shutter state
 */
void print_shutter_state(sendfun s){
	P("shutter ", s);
	switch (Shutter_State){
		case SHUTTER_INITIALIZED:
			P("testing", s);
		break;
		case SHUTTER_READY:
			P("ready", s);
		break;
		case SHUTTER_OPENED:
			P("opened", s);
		break;
		case SHUTTER_CLOSED:
			P("closed", s);
		break;
		case SHUTTER_OPENING:
		case SHUTTER_CLOSING:
			P("charged for ", s);
			if(Shutter_State == SHUTTER_OPENING) P("opening", s);
			else P("closing", s);
		break;
		case SHUTTER_PROC_OPENING:
		case SHUTTER_PROC_CLOSING:
			P("in process", s);
		break;
		default: // not ready or error
			if(shutter_error()){ // ERR==0 -> wire breakage or something else
				P("error", s);
			}else
				P("not initialised or broken", s);
	}
	newline(s);
}

/**
 * Try to open/close shutter depending on argument:
 * 		state == SHUTTER_CLOSING - try to close
 * 		state == SHUTTER_OPENING - try to open
 */
void shutter_try(shutter_state state){
	switch (Shutter_State){ // right states are only OPENED, CLOSED & READY
		case SHUTTER_OPENED:
			if(state == SHUTTER_OPENING){ // try to open opened shutter?
				P("alerady opened\n", lastsendfun);
				return;
			}
		break;
		case SHUTTER_CLOSED:
			if(state == SHUTTER_CLOSING){ // try to close closed?
				P("alerady closed\n", lastsendfun);
				return;
			}
		break;
		case SHUTTER_READY:
			; // All OK
		break;
		default:
			print_shutter_state(lastsendfun);
			return;
	}
	Shutter_State = state; // all OK, now we can change current state of shutter
}


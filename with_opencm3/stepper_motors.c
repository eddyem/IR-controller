/*
 * stepper_motors.c - moving of stepper motors
 *
 * Copyright 2014 Edward V. Emelianoff <eddy@sao.ru>
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

static uint8_t timers_activated[2] = {0, 0}; // flag of activated timers
uint16_t Motor_period[2] = {10000, 10000}; // near 100 steps per second
volatile uint8_t timer_flag[2] = {0,0};
// amount of steps for each motor
volatile uint32_t Motor_steps[5] = {0, 0, 0, 0, 0};
// flag of active motor
volatile uint8_t Motor_active[5] = {0, 0, 0, 0, 0};


/**
 * Setup stepper motors' timer Tim
 * N == 0 for TIM3, == 1 for TIM4
 */
static void setup_timer(uint8_t N){
	uint32_t Tim;
	switch (N){
		case 0:
			Tim = TIM3;
			nvic_enable_irq(NVIC_TIM3_IRQ);
		break;
		case 1:
			Tim = TIM4;
			nvic_enable_irq(NVIC_TIM4_IRQ);
		break;
		default:
		return;
	}
	timer_reset(Tim);
	// timers have frequency of 2MHz, 2 pulse == 1 microstep
	// 36MHz of APB1
	timer_set_mode(Tim, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	// 72MHz div 36 = 2MHz
	timer_set_prescaler(Tim, 35); // prescaler is (div - 1)
	timer_continuous_mode(Tim); // automatically reload
	timer_enable_preload(Tim); // force changing period
	timer_set_period(Tim, Motor_period[N] - 1);
	timer_enable_update_event(Tim);
	timer_enable_irq(Tim, TIM_DIER_UIE); // update IRQ enable
	timer_enable_counter(Tim);
	timers_activated[N] = 1;
	lastsendfun('3' + N);
	MSG(" timer\n");
}

/**
 * Set up motors pins & activate timers 3 & 4
 * Timer3 gives ticks to motors 1..3 (turrets)
 * Timer4 gives ticks to motors 4,5 (long & short stages)
 *
 * Timers are always work, stopping motors can be done by EN
 * Timers simply works as counters, no PWM mode
 */
void steppers_init(){
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN |
			RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN);
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN);
	// timer pins
	gpio_set_mode(MOTOR_TIM1_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, MOTOR_TIM1_PIN);
	gpio_set_mode(MOTOR_TIM2_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, MOTOR_TIM2_PIN);
	// EN pins
	gpio_set_mode(MOTOR_EN_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, MOROT_EN_MASK);
	// DIR pins
	gpio_set_mode(MOTOR_DIR_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, MOROT_DIR_MASK);
	setup_timer(0);
	setup_timer(1);
}

/**
 * Move motor Motor_number to User_value steps
 */
void move_motor(uint8_t num, int32_t steps){
	if(steps == 0) return;
	// check whether motor is moving
	if(Motor_active[num]){
		MSG("err: moving\r\n");
		return;
	}
	if(steps < 0){
		steps = -steps;
		gpio_set(MOTOR_DIR_PORT, MOTOR_DIR_PIN(num)); // set DIR bit to rotate ccw
	}else
		gpio_clear(MOTOR_DIR_PORT, MOTOR_DIR_PIN(num)); // reset DIR bit
	Motor_steps[num] = steps << 4; // multiply by 16 to get usteps count
	Motor_active[num] = 1;
	gpio_set(MOTOR_EN_PORT, MOTOR_EN_PIN(num)); // activate motor
}

void stop_motor(uint8_t num){
	if(!Motor_active[num]) return;
	MSG("stop motor ");
	lastsendfun('0' + num);
	MSG("\r\n");
	gpio_clear(MOTOR_EN_PORT, MOTOR_EN_PIN(num));
	Motor_active[num] = 0;
}

/**
 * Check flags set by timers & do next:
 *  - decrease step counter if it isn't zero;
 *  - stop motor if counter is zero but motor still active
 */
void process_stepper_motors(){
	int i, j;
	const uint32_t ports[] = {MOTOR_TIM1_PORT, MOTOR_TIM2_PORT};
	const uint32_t pins[] = {MOTOR_TIM1_PIN, MOTOR_TIM2_PIN};
	const uint8_t startno[] = {0, 3};
	const uint8_t stopno[]  = {3, 5};
	for(j = 0; j < 2; j++){
		if(timer_flag[j]){
			timer_flag[j] = 0;
			gpio_toggle(ports[j], pins[j]); // change clock state
			if(gpio_get(ports[j], pins[j])){ // positive pulse - next microstep
				for(i = startno[j]; i < stopno[j]; i++){ // check motors
					if(Motor_steps[i]) Motor_steps[i]--;
					else if(Motor_active[i]){ // stop motor - all done
						stop_motor(i);
					}
				}
			}
		}
	}
}

/**
 * Stop timers; turn off motor voltage
 *
void stop_timer(){
	// disable IRQs & stop timer
	TIM_Cmd(SM_Timer, DISABLE);
	TIM_ITConfig(SM_Timer, TIM_IT_Update, DISABLE);
	// turn off power
	SM_EN_GPIOx->BRR = SM_EN_PINS; // reset all EN bits
	SM_PUL_GPIOx->BRR = SM_PUL_PIN; // reset signal on PUL
	timers_activated = 0;
	Motor_steps = 0;
}*/

/**
 * Sets motor period to user value & refresh timer
 * @param num - number of motor
 * @param period - period of one STEP in microseconds
 */
void set_motor_period(uint8_t num, uint16_t period){
	uint32_t Tim, N;
	switch (num){
		case 1:
		case 2:
		case 3:
			Tim = TIM3;
			N = 0;
		break;
		case 4:
		case 5:
			Tim = TIM4;
			N = 1;
		break;
		default:
			MSG("err: bad motor");
			return;
	}
	period >>= 4; // divide by 4 to get 16usteps for one step
	if(period == 0) Motor_period[N] = 1;
	else Motor_period[N] = period;
	if(!timers_activated[N]) setup_timer(N);
	else timer_set_period(Tim, period);
}

/*
 * Interrupts: just set flag
 */
void tim3_isr(){
	if(timer_get_flag(TIM3, TIM_SR_UIF)){
		// Clear compare interrupt flag
		timer_clear_flag(TIM3, TIM_SR_UIF);
		timer_flag[0] = 1;
	}
}

void tim4_isr(){
	if(timer_get_flag(TIM4, TIM_SR_UIF)){
		// Clear compare interrupt flag
		timer_clear_flag(TIM4, TIM_SR_UIF);
		timer_flag[1] = 1;
	}
}

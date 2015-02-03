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
#include "stepper_motors.h"

//    TODO:  function "move motor to given position"

static uint8_t timers_activated[2] = {0, 0}; // flag of activated timers
uint16_t Motor_period[2] = {1300, 1300}; // one step per 1.3ms
uint32_t Turrets_pause = 2 * TURRETS_PAUSE_US / 1300; // pause in half-steps
volatile uint8_t timer_flag[2] = {0,0};
// amount of steps for each motor
volatile uint32_t Motor_steps[5] = {0, 0, 0, 0, 0};
// flag of active motor
volatile uint8_t Motor_active[5] = {0, 0, 0, 0, 0};
/*
 * Wait flags: if non-zero, flag just decremented
 * (we need it to wait a little on turrets' fixed positions to omit Halls' histeresis)
 */
uint8_t waits[5] = {0,0,0,0,0};
// Halls & end-switches values on previous step
uint8_t lastpos[5] = {0,0,0,0,0};
// number of position to move turret or stage, zero to move only for N given steps
uint8_t move2pos[5] = {0,0,0,0,0};

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
	timer_disable_preload(Tim); // force changing period
	timer_set_period(Tim, Motor_period[N] - 1);
	timer_enable_update_event(Tim);
	timer_enable_irq(Tim, TIM_DIER_UIE); // update IRQ enable
	timer_enable_counter(Tim);
	timers_activated[N] = 1;
#ifdef EBUG
	lastsendfun('3' + N);
	MSG(" timer\n");
#endif
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
	// WARNING! EN pins would be shortened to GND in case of overcurrent/overheating
	// so, they should be pull-up inputs in active mode & pull-down inputs in inactive mode!!!
	gpio_set_mode(MOTOR_EN_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_INPUT_PULL_UPDOWN, MOTOR_EN_MASK);
	gpio_clear(MOTOR_EN_PORT, MOTOR_EN_MASK);
	// DIR pins
	gpio_set_mode(MOTOR_DIR_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, MOTOR_DIR_MASK);
	setup_timer(0);
	setup_timer(1);
	// Now setup Halls & end-switches
	SETUP_ESW();
}

/**
 * Test end-switches &
 * @param num    - motor number
 * @param curpos - end-switches data for motor
 * @return 0 if we can move further, 1 if there's the end
 */
uint8_t test_stages_endpos(uint8_t num, uint8_t curpos){
	if(curpos == 0 || num < 3) return 0;
	// end-switches numbers for stages
	const uint8_t stage_plus[2] = {STAGE_CHECK(3, PLUS), STAGE_CHECK(4, PLUS)};
	const uint8_t stage_minus[2] = {STAGE_CHECK(3, MINUS), STAGE_CHECK(4, MINUS)};
	uint8_t negative_dir = 0;
	num -= 3; // convern num to index in arrays
	if((uint16_t)GPIO_IDR(MOTOR_EN_PORT) & MOTOR_EN_PIN(num)){ // negative direction
		negative_dir = 1;
	}
	if(stage_plus[num] == curpos){ // we are on "+" end-switch
		if(!negative_dir){ // and wanna move to "+"
			ERR("End-switch +\n");
			return 1;
		}
	}else if(stage_minus[num] == curpos){ // we are on "-" end-switch
		if(negative_dir){ // and wanna move to "-"
			ERR("End-switch -\n");
			return 1;
		}
	}else{ // error: WTF is going up? curpos != 2 or 1
		ERR("Wrong current position: ");
		print_int(curpos, lastsendfun);
		lastsendfun('\n');
		return 1;
	}
	return 0;
}

/**
 * Return value of current Hall/end-switches position
 * (converted to normal uint8_t, 0 == none
 */
uint8_t check_ep(uint8_t num){
	switch (num){
		case 0:
			return CHECK_EP(0);
		break;
		case 1:
			return CHECK_EP(1);
		break;
		case 2:
			return CHECK_EP(2);
		break;
		case 3:
			return CHECK_EP(3);
		break;
		case 4:
			return CHECK_EP(4);
		break;
	}
	return 0;
}

/**
 * Move motor Motor_number to User_value steps
 */
void move_motor(uint8_t num, int32_t steps){
	uint8_t curpos, negative_dir = 0;
	if(steps == 0) return;
	// check whether motor is moving
	if(Motor_active[num]){
		ERR("moving\n");
		return;
	}
#ifdef EBUG
	MSG("move ");
	lastsendfun('0' + num);
	MSG(" to ");
	print_int(steps, lastsendfun);
	MSG("\n");
#endif
	if(steps < 0){
		negative_dir = 1;
		steps = -steps;
	}
	curpos = check_ep(num);
	lastpos[num] = curpos;
	if(negative_dir){
		gpio_set(MOTOR_DIR_PORT, MOTOR_DIR_PIN(num)); // set DIR bit to rotate ccw
	}else{
		gpio_clear(MOTOR_DIR_PORT, MOTOR_DIR_PIN(num)); // reset DIR bit
	}
	if(test_stages_endpos(num, curpos)) return; // error: we can't move
	// set all flags and variables
	Motor_steps[num] = steps; // we run in full-step mode!
	waits[num] = 0;
	Motor_active[num] = 1;
	gpio_set(MOTOR_EN_PORT, MOTOR_EN_PIN(num));
}

void stop_motor(uint8_t num){
	if(!Motor_active[num]) return;
#ifdef EBUG
	MSG("stop motor ");
	lastsendfun('0' + num);
	MSG("\n");
#endif
	gpio_clear(MOTOR_EN_PORT, MOTOR_EN_PIN(num));
	Motor_active[num] = 0;
	if(num < 3){ // this is a turret
		move2pos[num] = 0; // reset target position value
		if(check_ep(num) == 0){ // a turret is out of fixed position
			ERR("stop out of position\n");
		}
	}
}


/*
 * Wa cannot use EXTI because multiplexer doesn't allow simultaneous interrupts
 * handling on the same bits of different ports (e.g. PB7 & PD7)
 * So, I need to check registers' state before each step!
 */

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
	uint8_t curpos;
	for(j = 0; j < 2; j++){
		if(timer_flag[j]){
			timer_flag[j] = 0;
			gpio_toggle(ports[j], pins[j]); // change clock state
			if(!gpio_get(ports[j], pins[j])){ // negative pulse - omit this half-step
				continue;
			}
			for(i = startno[j]; i < stopno[j]; i++){ // check motors
				if(Motor_active[i] == 0) continue; // inactive motor
				curpos = check_ep(i);
				if(Motor_steps[i] == 0){ // end of moving
					stop_motor(i); // even if this is a turret with move2pos[i]!=0 we should stop
						//(what if there's some slipping or so on?)
				}else{ // we should move further
					if(waits[i]){ // waiting for position stabilisation
						waits[i]--;
						if(waits[i]) continue; // there's more half-steps to skip
						lastpos[i] = curpos;
						// tell user current position
						MSG("position: ");
						print_int(curpos, lastsendfun);
						lastsendfun('\n');
						// turn on motor after pause
						gpio_set(MOTOR_EN_PORT, MOTOR_EN_PIN(i));
						if(j == 1){ // this is a linear stage
							if(test_stages_endpos(i, curpos)){ // this is the end of way
								stop_motor(i);
							}
						}else{ // this is a turret
							if(move2pos[i]){ // we should move to specific position
								if(curpos == move2pos[i]){ // we are on position
									stop_motor(i);
								}else{ // add some steps to move to next position
									Motor_steps[i] += TURRETS_NEXT_POS_STEPS;
								}
							}
						}
					}else{
						// check for overcurrent: if MOTOR_EN_PIN == 0
						if(!gpio_get(MOTOR_EN_PORT, MOTOR_EN_PIN(i))){
							ERR("overcurrent\n");
							stop_motor(i);
							continue;
						}
						if(lastpos[i] != curpos){ // transition process
							if(lastpos[i] == 0){ // come towards position
								waits[i] = Turrets_pause;
								// turn off motor while a pause
								gpio_clear(MOTOR_EN_PORT, MOTOR_EN_PIN(i));
								continue;
							}
							lastpos[i] = curpos;
						}
						Motor_steps[i]--;
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
 * @param period - period of one MICROSTEP in microseconds
 */
void set_motor_period(uint8_t num, uint16_t period){
	uint32_t Tim, N;
	switch (num){
		case 1:
		case 2:
		case 3:
			Tim = TIM3;
			N = 0;
			Turrets_pause = 2 * TURRETS_PAUSE_US / period + 1; // pause in half-steps
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

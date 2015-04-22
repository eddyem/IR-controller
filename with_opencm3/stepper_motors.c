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
static uint16_t Motor_period[2] = {3000, 2000};
static uint32_t Turrets_pause = 2 * TURRETS_PAUSE_US / 3000; // pause in half-steps
static volatile uint8_t timer_flag[2] = {0,0};
// amount of steps for each motor
static volatile uint32_t Motor_steps[5] = {0, 0, 0, 0, 0};
// absolute value of current position, usefull for stages
static volatile int32_t Motor_abs_steps[5] = {0, 0, 0, 0, 0};
// increments that will be added each step to Motor_abs_steps (+1/-1)
static int8_t Motor_step_increment[5] = {1,1,1,1,1};
// flag of active motor
static volatile uint8_t Motor_active[5] = {0, 0, 0, 0, 0};
/*
 * Wait flags: if non-zero, flag just decremented
 * (we need it to wait a little on turrets' fixed positions to omit Halls' histeresis)
 */
static uint8_t waits[5] = {0,0,0,0,0};
// acceleration: if non-zero we will omit N steps after each step & decrement accell value
static uint8_t accel[5] = {0,0,0,0,0};
// Halls & end-switches values on previous step
static uint8_t lastpos[5] = {0,0,0,0,0};
// number of position to move turret or stage, zero to move only for N given steps
uint8_t move2pos[5] = {0,0,0,0,0};
// number of positions passed for given
static uint8_t positions_pass[3] = {0,0,0};
// maximum amount of positions passed to reach given
#define MAX_POSITIONS_PASS    (8)

// multipliers for linear acceleration (in reverce order)
static const uint8_t accel_mults[16] = {1, 2, 2, 2, 2, 2, 2, 3, 3, 4, 4, 5, 6, 8, 10, 16};


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
	timer_set_prescaler(Tim, 35); // prescaler is (div - 1), 2pulse == 1 step
	timer_continuous_mode(Tim); // automatically reload
	timer_disable_preload(Tim); // force changing period
	timer_set_period(Tim, Motor_period[N] - 1);
	timer_enable_update_event(Tim);
	timer_enable_irq(Tim, TIM_DIER_UIE); // update IRQ enable
	timer_enable_counter(Tim);
	timers_activated[N] = 1;
#ifdef EBUG
	if(mode == BYTE_MODE){
		lastsendfun('3' + N);
		P(" timer\n", lastsendfun);
	}
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
	// so, when active they should be opendrain outputs with 100k external resistor to +5V or pullup inputs!!!
	// inactive: opendrain output
	gpio_set_mode(MOTOR_EN_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_OPENDRAIN, MOTOR_EN_MASK);
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
	if((uint16_t)(GPIO_ODR(MOTOR_DIR_PORT) & MOTOR_DIR_PIN(num))){ // negative direction
		negative_dir = 1;
	}
	num -= 3; // convern num to index in arrays
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
		if(mode == BYTE_MODE){
			print_int(curpos, lastsendfun);
			lastsendfun('\n');
		}
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
 * test 12V voltage
 * if it is less than MOTORS_VOLTAGE_THRES, return 1 & show err message
 * else return 0
 */
uint8_t undervoltage_test(int thres){
	int voltage = power_voltage();
	if(voltage < thres){
		ERR("undervoltage!\n");
		if(mode == LINE_MODE){
			P("[ " STR_MOTORS_VOLTAGE " ", lastsendfun);
			print_int(voltage, lastsendfun);
			P(" ]\n", lastsendfun);
		}
		return 1;
	}
	return 0;
}

/**
 * Move motor Motor_number to User_value steps
 * return 0 if motor is still moving
 */
uint8_t move_motor(uint8_t num, int32_t steps){
	uint8_t curpos, negative_dir = 0, N_active_in_group = 0;
	if(steps == 0) return 0;
	// check whether motor is moving
/*	if(Motor_active[num]){
		ERR("moving\n");
		return 0;
	}*/
	// don't move motors if there's no power enough
	if(undervoltage_test(MOTORS_VOLTAGE_THRES)) return 0;
	if(num < 3){
		for(curpos = 0; curpos < 4; curpos++)
			if(Motor_active[curpos]) N_active_in_group++;
	}else{
		if(Motor_active[3] || Motor_active[4]) N_active_in_group = 1;
	}
	if(N_active_in_group){ // we can't move: there's any active motor in group
		ERR("moving\n");
		return 0;
	}
#ifdef EBUG
	if(mode == BYTE_MODE){
		P("move ", lastsendfun);
		lastsendfun('0' + num);
		P(" to ", lastsendfun);
		print_int(steps, lastsendfun);
		lastsendfun('\n');
	}
#endif
	if(steps < 0){
		negative_dir = 1;
		Motor_step_increment[num] = -1;
		steps = -steps;
	}else
		Motor_step_increment[num] = 1;
	curpos = check_ep(num);
	lastpos[num] = curpos;
	if(negative_dir){
		gpio_set(MOTOR_DIR_PORT, MOTOR_DIR_PIN(num)); // set DIR bit to rotate ccw
	}else{
		gpio_clear(MOTOR_DIR_PORT, MOTOR_DIR_PIN(num)); // reset DIR bit
	}
	if(test_stages_endpos(num, curpos)){ // error: we can't move
		stop_motor(num); // say about it
		return 0;
	}
	// set all flags and variables
	Motor_steps[num] = steps; // we run in full-step mode!
	waits[num] = 0;
	accel[num] = START_MOTORS_ACCEL_IDX_4;
	Motor_active[num] = 1;
	if(num < 3) // this is turret -> reset counter of passed positions
		positions_pass[num] = 0;
	// pullup input when active
	gpio_set_mode(MOTOR_EN_PORT, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_PULL_UPDOWN, MOTOR_EN_PIN(num));
	gpio_set(MOTOR_EN_PORT, MOTOR_EN_PIN(num));
/*
	P("set: ", lastsendfun);
	print_int(GPIO_ODR(MOTOR_EN_PORT) & MOTOR_EN_MASK, lastsendfun);
	P(", get: ", lastsendfun);
	print_int(GPIO_IDR(MOTOR_EN_PORT) & MOTOR_EN_MASK, lastsendfun);
	lastsendfun('\n');
*/
	return 1;
}


void stop_motor(uint8_t num){
	const uint8_t stage_minus[2] = {STAGE_CHECK(3, MINUS), STAGE_CHECK(4, MINUS)};
	//if(!) return;
	MSG("stop motor ", "[ " STR_STOP_ALL_MOTORS " ");
	if(mode != BINARY_MODE){
		lastsendfun('0' + num);
		lastsendfun(' ');
	}
	// this function could be called simply to check motors' position
	// so, we should check wether motor is active before changing EN state
	if(Motor_active[num]){
		if(!gpio_get(MOTOR_EN_PORT, MOTOR_EN_PIN(num)) && mode == LINE_MODE)
			P("HEAT ", lastsendfun);
		gpio_set_mode(MOTOR_EN_PORT, GPIO_MODE_OUTPUT_2_MHZ,
			GPIO_CNF_OUTPUT_OPENDRAIN, MOTOR_EN_PIN(num));
		gpio_clear(MOTOR_EN_PORT, MOTOR_EN_PIN(num));
		Motor_active[num] = 0;
	}
	uint8_t curpos = check_ep(num);
	// set absolute counter to zero on 1st position for turrets or on MINUS EP for stages
	if(num < 3){ // this is a turret
		move2pos[num] = 0; // reset target position value
		if(curpos == 1){
			Motor_abs_steps[num] = 0;
		}else{
			if(curpos == 0) // a turret is out of fixed position
				MSG("stop out of position", "ERR ");
		}
	}else{ // linear stage
		if(curpos == stage_minus[num-3]){
			Motor_abs_steps[num] = 0;
		}
	}
	Motor_steps[num] = 0;
	BYTE_MSG(" absolute steps: ");
	print_int(Motor_abs_steps[num], lastsendfun);
	if(mode == LINE_MODE) P(" ]", lastsendfun);
	lastsendfun('\n');
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
	//static uint8_t showcurpos[5] = {0,0,0,0,0};
	uint8_t curpos;
	const uint32_t Tim[2] = {TIM3, TIM4};
	for(j = 0; j < 2; j++){
		// new period of motors' timer -- maximum value for all periods in group
		uint16_t new_period = 0;
		if(timer_flag[j]){
			timer_flag[j] = 0;
			uint8_t is_active = 0;
			for(i = startno[j]; i < stopno[j]; i++)
				if(Motor_active[i]) is_active = 1;
			if(!is_active) continue; // don't generate clock pulses when there's no moving motors
			if(undervoltage_test(MOTORS_VOLTAGE_ALERT)){ // UNDERVOLTAGE! Stop all active motors
				for(i = 0; i < 5; i++)
					if(Motor_active[i]) stop_motor(i);
				return;
			}
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
						uint8_t got_new_position = 0;
						waits[i]--;
						if(waits[i]) continue; // there's more half-steps to skip
						// tell user current position if we was stopped at fixed pos
						if(lastpos[i] == 0 && curpos != 0){
							got_new_position = 1;
							MSG("position of motor ", "[ " STR_ENDSW_STATE " ");
							print_int(i, lastsendfun);
							lastsendfun(' ');
							print_int(curpos, lastsendfun);
							if(mode == LINE_MODE) P(" ]", lastsendfun);
							lastsendfun('\n');
						}
						lastpos[i] = curpos;
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
								}else if(got_new_position){ // add some steps to move to next position
									if(++positions_pass[i] > MAX_POSITIONS_PASS){
										ERR("Can't reach given position");
										stop_motor(i);
									}else
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
								if(j == 0){ // this is a turret: make pause & prepare acceleration for start
									waits[i] = Turrets_pause;
									accel[i] = START_MOTORS_ACCEL_IDX_4;
								}else{
									waits[i] = 1;
								}
								// turn off motor while a pause (turret will be locked at fixed position by spring)
								// for this short pause we can simply do a pulldown
								gpio_clear(MOTOR_EN_PORT, MOTOR_EN_PIN(i));
								continue;
							}
							lastpos[i] = curpos;
						}
						Motor_steps[i]--;
						// change value of current motor's position
						Motor_abs_steps[i] += Motor_step_increment[i];
						if(accel[i]){ // we are starting
							uint32_t NP = (uint32_t)Motor_period[j] * accel_mults[(accel[i]--)/4];
							if(NP > 0xffff) NP = 0xffff;
							if(new_period < NP) new_period = (uint16_t)NP;
						}
					}
				}
			}
			if(new_period){ // we have to change motors' speed when accelerating
				timer_set_period(Tim[j], new_period);
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
			ERR("bad motor");
			return;
	}
	if(period == 0) Motor_period[N] = 1;
	else Motor_period[N] = period;
	if(!timers_activated[N]) setup_timer(N);
	else timer_set_period(Tim, period);
}

void get_motors_position(){
	uint8_t i;
	for(i = 0; i < 5; i++){
		MSG("position of ", "[ " STR_MOTOR_POSITION " ");
		lastsendfun(i+'0');
		MSG(" is ", " ");
		print_int(Motor_abs_steps[i], lastsendfun);
		if(Motor_active[i]){
			lastsendfun(' ');
			P("moving", lastsendfun);
		}
		if(mode == LINE_MODE) P(" ]", lastsendfun);
		lastsendfun('\n');
	}
}

/**
 * displays periods of both generators
 */
void show_motors_period(sendfun s){
	P("[ " STR_SHOW_PERIOD " ", s);
	print_int((int32_t)Motor_period[0],s);
	s(' ');
	print_int((int32_t)Motor_period[1],s);
	P(" ]\n", s);
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

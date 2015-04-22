/*
 * leds.c - manage on-board LED in software PWM mode
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

#include "leds.h"

// global variables for software PWM emilation
uint8_t  led_state = LEDSTATE_OFF;
uint16_t led_blink_on = 0;
uint16_t led_blink_off = 0;
uint16_t led_ticks_on = 0;
uint16_t led_ticks_off = 0;
uint8_t  duty_cycle = 4;

uint8_t LED_GetState(){
	return led_state;
}
void LED_Duty(uint8_t duty){
	duty_cycle = duty;
	if(led_state == LEDSTATE_BLINK)
		LED_OnBlink();
}
void LED_DutyPlus(){
	if(duty_cycle < 7) duty_cycle++;
	if(led_state == LEDSTATE_BLINK)
		LED_OnBlink();
}
void LED_DutyMinus(){
	if(duty_cycle > 0) duty_cycle--;
	if(led_state == LEDSTATE_BLINK)
		LED_OnBlink();
}
uint8_t LED_GetBlinkState(uint16_t *blink_on, uint16_t *blink_off){
	*blink_on = led_blink_on;
	*blink_off = led_blink_off;
	return led_state;
}
void LED_On(){
	led_state = LEDSTATE_ON;
	led_blink_on = 0;
	led_blink_off = 0;
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}
void LED_Off(){
	led_state = LEDSTATE_OFF;
	GPIO_SetBits(GPIOC, GPIO_Pin_12);
}
void LED_OnBlink(){
	led_blink_off = 1 << duty_cycle;
	led_blink_on = 0xff - led_blink_off;
	led_ticks_on = 0;
	led_ticks_off = 0;

	if(led_blink_off == 0){
		LED_On();
		return;
	}
	if(led_blink_on == 0)
	{
		LED_Off();
		return;
	}
	led_state = LEDSTATE_BLINK;
}
void LED_SysTick_Handler(){
	if(led_state != LEDSTATE_BLINK) return;
	if(led_ticks_on == 0)
		GPIO_SetBits(GPIOC, GPIO_Pin_12);
	if(led_ticks_on <= led_blink_on)	{
		led_ticks_on++;
		return;
	}
	if (led_ticks_off == 0){
		GPIO_ResetBits(GPIOC, GPIO_Pin_12);
	}
	if(led_ticks_off <= led_blink_off){
		led_ticks_off++;
		return;
	}
	led_ticks_on = 0;
	led_ticks_off = 0;
}

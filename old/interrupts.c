/*
 * interrupts.c - interrupts handlers
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

// sync USART buffer to USB every IN_FRAME_INTERVAL SOF packets
#define IN_FRAME_INTERVAL			5

// buffer for input data
uint8_t USB_Rx_Buffer[SomeDev_DATA_SIZE];

void NMI_Handler(){}
void HardFault_Handler(){while (1){};}
void MemManage_Handler(){while (1){};}
void BusFault_Handler(){while (1){};}
void UsageFault_Handler(){while (1){};}
void SVC_Handler(){}
void DebugMon_Handler(){}
void PendSV_Handler(){}
void SDIO_IRQHandler(){}

uint16_t systick_prescale_count = 0; // prescale counter for Delay()
// SysTick: soft PWM for LED, delay counter
void SysTick_Handler(){
	systick_prescale_count++;
	LED_SysTick_Handler();
	if(systick_prescale_count < SYSTICK_PRESCALE)
		return;
	systick_prescale_count = 0;
	Delay_SysTickHandler(); // increment delay counter
}

/*
 * peripherial interrupts
 */

// button EXTernal Interrupt
void EXTI0_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		if(LED_GetState() != LEDSTATE_BLINK)
			LED_OnBlink();
		else
			LED_On();
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
// Hall sensor EXTernal Interrupt
void EXTI2_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line2) != RESET){
		FLAGS |= FLAG_PRINTHALL;
		EXTI_ClearITPendingBit(EXTI_Line2);
	}
}

/*
 * USB interrupts
 */

/*
// USB High Priority / CAN TX
void USB_HP_CAN1_TX_IRQHandler(){
	CTR_HP();
}*/

// USB Low Priority / CAN RX0
void USB_LP_CAN1_RX0_IRQHandler(){
	USB_Istr();
}

// Callback to put data to host - flush buffer
void EP1_IN_Callback (){
	Handle_USBAsynchXfer();
}

// Callback to get data from host - read it
void EP3_OUT_Callback(){
	uint16_t USB_Rx_Cnt;
	// USB_Rx_Cnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);
	USB_Rx_Cnt = GetEPRxCount(EP3_OUT);
	PMAToUserBufferCopy(USB_Rx_Buffer, ENDP3_RXADDR, USB_Rx_Cnt);
	// end of USB_SIL_Read()
	usb_handle_command(USB_Rx_Cnt);
	SetEPRxValid(ENDP3);
}

// Start Of Frame callback (11-bit frame number)
// we'll flush data in USB TX buffer every IN_FRAME_INTERVAL frames
void SOF_Callback(){
	static uint32_t FrameCount = 0;
	if(bDeviceState == CONFIGURED){
		if(FrameCount++ == IN_FRAME_INTERVAL){
			// Reset the frame counter
			FrameCount = 0;
			// Check the data to be sent through IN pipe
			Handle_USBAsynchXfer();
		}
	}
}

void TIM1_CC_IRQHandler(){
	if(TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET){
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
		if(--Motor_steps) return;
		stop_timer1();
		P("motor rdy");
		newline();
	}
}

static uint8_t ZWercntr = 0;
void TIM2_IRQHandler(){
	int32_t cnt1, cnt2;
	if(TIM_GetITStatus(ZW_Timer, TIM_IT_CC3) != RESET){ // error: no data
		TIM_ClearITPendingBit(ZW_Timer, TIM_IT_CC3);
		if(ZW_status != ZW_START || ++ZWercntr > 10){ // wait up to .5s
			ZWercntr = 0;
			ZW_nodata();
		}
	}
	if(TIM_GetITStatus(ZW_Timer, TIM_IT_CC2) != RESET){ // data IRQ
		if(ZW_status == ZW_START) ZW_status = ZW_PROCESS;
		TIM_ClearITPendingBit(ZW_Timer, TIM_IT_CC2);
		TIM_ClearITPendingBit(ZW_Timer, TIM_IT_Update);
		cnt1 = TIM_GetCapture1(ZW_Timer); //width
		cnt2 = TIM_GetCapture2(ZW_Timer); //pulse
		cnt1 = (cnt1*100)/cnt2;
		if(cnt1)
			ZW_fill_next_bit(cnt1);
	}
}

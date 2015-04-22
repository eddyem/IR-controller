/*
 * hw_config.c - USB & USART configuration
 *
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

#define USART1_IRQHandler Dummy
#define USART2_IRQHandler Dummy
#define USART3_IRQHandler Dummy
void Dummy(){};

// Configure ports
__IO uint16_t ADC_value; // ADC DMA value

/**
 * Configure GPIO ports (all pins have 2MHz speed)
 * @param mode  - port mode
 * @param GPIOx - port address
 * @param pin   - pin[s] to configure
 * @param spd   - GPIO speed
 */
void GPIO_pin_config_common(GPIOMode_TypeDef mode, GPIO_TypeDef* GPIOx, uint16_t pin, GPIOSpeed_TypeDef spd){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = spd;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = mode;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

// default - slow
#define GPIO_pin_config(a,b,c) GPIO_pin_config_common(a,b,c, GPIO_Speed_2MHz)
#define GPIO_pin_config_fast(a,b,c) GPIO_pin_config_common(a,b,c, GPIO_Speed_50MHz)

void Ports_Config(){
	EXTI_InitTypeDef EXTI_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;
	// enable AFIO clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// enable clocks for used ports
	RCC_APB2PeriphClockCmd(	  RCC_APB2Periph_GPIOA
							| RCC_APB2Periph_GPIOB
							| RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD
								,ENABLE);
	/*
	 * Button (A0)
	 */
	GPIO_pin_config(GPIO_Mode_IN_FLOATING, GPIOA, GPIO_Pin_0);
	// Connect Button EXTI Line to Button GPIO Pin
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	// Configure Button EXTI line
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	// Enable Button EXTI Interrupt
	NVIC_EnableIRQ(EXTI0_IRQn);
	/*
	 * LED (PC12) & USB software connect-disconnect (PC11) pins
	 */
	GPIO_pin_config(GPIO_Mode_Out_PP, GPIOC, GPIO_Pin_12 | GPIO_Pin_11);
	// put USB into disconnect state
	GPIO_SetBits(GPIOC, GPIO_Pin_11);
	/*
	 * Hall sensor (PD2)
	 */
	GPIO_pin_config(GPIO_Mode_IN_FLOATING, Hall_GPIOx, Hall_PIN);
	// Configure Hall EXTI
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	// Enable Hall IRQ
	NVIC_EnableIRQ(EXTI2_IRQn);
	/*
	 * ADC (PB0)
	 */
	// 0. Configure ADC8 (PB0) as analog input (clocking GPIOB sat on in onewire.c)
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	GPIO_pin_config(GPIO_Mode_AIN, GPIOB, GPIO_Pin_0);
	// 1. DMA for converted value (DMA1 clocking sat on at onewire.c)
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_value;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	// 2. ADC1 config
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	// Connect ADC to ADC8 (PB0),
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);
	// Enable ADC1 DMA
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	// Calibration of ADC1
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // turn conversion on
	/*
	 * ADG506 control port (PC0..4)
	 * PC0..PC3 - analog channel address, PC4 - analog enable switch
	 */
	GPIO_pin_config(GPIO_Mode_Out_PP, GPIOC, ((uint16_t)0x1f));
	//GPIO_pin_config(GPIO_Mode_Out_OD, GPIOC, ((uint16_t)0x1f));
	/*
	 * 1-wire
	 * PA2 - USART_2TX (for USART3 - PB10 or remap to other pins)
	 * PA3 - USART_2RX (for USART3 - PB11 or remap)
	 */
	GPIO_pin_config_fast(GPIO_Mode_AF_OD, GPIOB, GPIO_Pin_10); // to omit Shottke diode
	GPIO_pin_config_fast(GPIO_Mode_IN_FLOATING, GPIOB, GPIO_Pin_11);
	// turn on USART2 (USART3 - RCC_APB1Periph_USART3)
	// to remap pins run GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE)
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	/*
	 * Stepper motors
	 * PA8/TIM1_CH1 - PUL (step) for all motors
	 * PB1 - DIR for all motors
	 * PC5..PC9 - ENx - pins to activate proper motor
	 * all are push-pull
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // enable timer 1 clocking
	GPIO_pin_config(GPIO_Mode_AF_PP, SM_PUL_GPIOx, SM_PUL_PIN); // PA8 - PUL (by timer)
	GPIO_pin_config(GPIO_Mode_Out_PP, SM_DIR_GPIOx, SM_DIR_PIN); // PB1 - DIR
	// channels EN configured as open-drain output because (+) of "EN" on stepper driver side have +5V
	GPIO_pin_config(GPIO_Mode_Out_OD, SM_EN_GPIOx, SM_EN_PINS); // PC5..PC9 - Enx
	// enable timer1 IRQ
	NVIC_EnableIRQ(TIM1_CC_IRQn);
	/*
	 * ZacWire
	 * PA1 (TIM2_CH2) used to capture data by PWM input mode
	 * PB5 used for power management: TSic thermometers get power only for a measurement
	 * !!! When using a lot of such thermometers powering them is better with multiplexer
	 */
	// Power pin: 5v tolerant
	//GPIO_pin_config(GPIO_Mode_Out_PP, ZW_PWR_GPIOx, ZW_PWR_PIN);
	GPIO_pin_config(GPIO_Mode_Out_OD, ZW_PWR_GPIOx, ZW_PWR_PIN);
	ZW_PWR_GPIOx->BRR = ZW_PWR_PIN; // turn off power
	// data pin: floating input (PA8)
	GPIO_pin_config(GPIO_Mode_IN_FLOATING, ZW_DATA_GPIOx, ZW_DATA_PIN);
	// enable timer2 clocking & its IRQ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
}


// USB
uint8_t  USART_Rx_Buffer[USART_RX_DATA_SIZE];
uint32_t USART_Rx_ptr_in = 0;

// USB clock (48 MHz)
void Set_USBClock(void){
	// Select USBCLK source
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	// Enable the USB clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

// USB RX interrupts
void USB_Interrupts_Config(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
/*
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
*/
}


// Software disconnect
void USB_Cable_Config (FunctionalState NewState){
	if (NewState != DISABLE)
		GPIO_ResetBits(GPIOC, GPIO_Pin_11);
	else
		GPIO_SetBits(GPIOC, GPIO_Pin_11);
}

// Low power mode (if not required - just set flag)
void Enter_LowPowerMode(void){
	bDeviceState = SUSPENDED;
}

// enter full power mode (if not required - just set flag)
void Leave_LowPowerMode(void){
	DEVICE_INFO *pInfo = &Device_Info;
	if (pInfo->Current_Configuration != 0){
		bDeviceState = CONFIGURED;
	}else{
		bDeviceState = ATTACHED;
	}
}

// Clocks
void RCC_Config(void){
	ErrorStatus HSEStartUpStatus;
	// Configuration of SYSCLK, HCLK, PCLK2 É PCLK1
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if (HSEStartUpStatus == SUCCESS){
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		// PLLCLK = 8MHz * 9 = 72 MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		RCC_PLLCmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){}
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while(RCC_GetSYSCLKSource() != 0x08){}
	}else{
		// HSE start error - clock configuration will be wrong
		// maybe say about it?
		while (1){}
	}
}

// send data to USB
void Handle_USBAsynchXfer (void){
	uint16_t USB_Tx_ptr, USB_Tx_length;
	static uint32_t USART_Rx_ptr_out = 0, USART_Rx_length  = 0;

	if (USART_Rx_ptr_out == USART_RX_DATA_SIZE){
		USART_Rx_ptr_out = 0;
	}
	if(USART_Rx_ptr_out == USART_Rx_ptr_in){
		return;
	}
	if(USART_Rx_ptr_out > USART_Rx_ptr_in){ /* rollback */
		USART_Rx_length = USART_RX_DATA_SIZE - USART_Rx_ptr_out;
	}else{
		USART_Rx_length = USART_Rx_ptr_in - USART_Rx_ptr_out;
	}
	USB_Tx_ptr = USART_Rx_ptr_out;
	// if data length > packet size
	if (USART_Rx_length > SomeDev_DATA_SIZE){
		USB_Tx_length = SomeDev_DATA_SIZE;
		USART_Rx_ptr_out += SomeDev_DATA_SIZE;
		USART_Rx_length -= SomeDev_DATA_SIZE;
	}else{
		USB_Tx_length = USART_Rx_length;
		USART_Rx_ptr_out += USART_Rx_length;
		USART_Rx_length = 0;
	}
	UserToPMABufferCopy(&USART_Rx_Buffer[USB_Tx_ptr], ENDP1_TXADDR, USB_Tx_length);
	SetEPTxCount(ENDP1, USB_Tx_length);
	SetEPTxValid(ENDP1);
}


void USB_Send_Data(uint8_t data){
	USART_Rx_Buffer[USART_Rx_ptr_in] = data;
	USART_Rx_ptr_in++;
	if(USART_Rx_ptr_in >= USART_RX_DATA_SIZE){
		USART_Rx_ptr_in = 0;
	}
}

/*
bool USART_Config(void)
{

  switch (linecoding.format)
  {
    case 0:
      USART_InitStructure.USART_StopBits = USART_StopBits_1;
      break;
    case 1:
      USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
      break;
    case 2:
      USART_InitStructure.USART_StopBits = USART_StopBits_2;
      break;
    default :
    {
      USART_Config_Default();
      return (FALSE);
    }
  }

  switch (linecoding.paritytype)
  {
    case 0:
      USART_InitStructure.USART_Parity = USART_Parity_No;
      break;
    case 1:
      USART_InitStructure.USART_Parity = USART_Parity_Even;
      break;
    case 2:
      USART_InitStructure.USART_Parity = USART_Parity_Odd;
      break;
    default :
    {
      USART_Config_Default();
      return (FALSE);
    }
  }

  switch (linecoding.datatype)
  {
    case 0x07:
      USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      break;
    case 0x08:
      if (USART_InitStructure.USART_Parity == USART_Parity_No)
      {
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
      }
      else
      {
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
      }

      break;
    default :
    {
      USART_Config_Default();
      return (FALSE);
    }
  }

  USART_InitStructure.USART_BaudRate = linecoding.bitrate;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  initPORT(&USART_InitStructure);

  return (TRUE);
}
*/

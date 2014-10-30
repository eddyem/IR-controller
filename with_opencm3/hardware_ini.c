/*
 * hardware_ini.c - functions for HW initialisation
 *
 * Copyright 2014 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
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

/*
 * All hardware-dependent initialisation & definition should be placed here
 * and in hardware_ini.h
 *
 */

#include "main.h"
#include "hardware_ini.h"

volatile uint16_t ADC_value[8]; // ADC DMA value

/*
 * Configure SPI ports
 */
/*
 *      SPI1 remapped:
 * SCK  - PB3
 * MISO - PB4
 * MOSI - PB5
 */
void SPI1_init(){
	// enable AFIO & other clocking
	rcc_peripheral_enable_clock(&RCC_APB2ENR,
			RCC_APB2ENR_SPI1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN);
	// remap SPI1 (change pins from PA5..7 to PB3..5); also turn off JTAG
	gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_OFF, AFIO_MAPR_SPI1_REMAP);
	// SCK, MOSI - push-pull output
	gpio_set_mode(GPIO_BANK_SPI1_RE_SCK, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_SPI1_RE_SCK);
	gpio_set_mode(GPIO_BANK_SPI1_RE_MOSI, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_SPI1_RE_MOSI);
	// MISO - opendrain in
	gpio_set_mode(GPIO_BANK_SPI1_RE_MISO, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_SPI1_RE_MISO);
	spi_reset(SPI1);
	/* Set up SPI in Master mode with:
	 * Clock baud rate: 1/128 of peripheral clock frequency (APB2, 72MHz)
	 * Clock polarity: Idle High
	 * Clock phase: Data valid on 2nd clock pulse
	 * Data frame format: 8-bit
	 * Frame format: MSB First
	 */
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_128, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
		SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	nvic_enable_irq(NVIC_SPI1_IRQ); // enable SPI interrupt
}

/*
 *     SPI2:
 * SCK  - PB13
 * MISO - PB14
 * MOSI - PB15
 */
void SPI2_init(){
	// turn on clocking
	//rcc_periph_clock_enable(RCC_SPI2 | RCC_GPIOB);
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI2EN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN);
	// SCK, MOSI - push-pull output
	gpio_set_mode(GPIO_BANK_SPI2_SCK, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_SPI2_SCK);
	gpio_set_mode(GPIO_BANK_SPI2_MOSI, GPIO_MODE_OUTPUT_50_MHZ,
		GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_SPI2_MOSI);
	// MISO - opendrain in
	gpio_set_mode(GPIO_BANK_SPI2_MISO, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_SPI2_MISO);
	spi_reset(SPI2);
	/* Set up SPI in Master mode with:
	 * Clock baud rate: 1/64 of peripheral clock frequency (APB1, 36MHz)
	 * Clock polarity: Idle High
	 * Clock phase: Data valid on 2nd clock pulse
	 * Data frame format: 8-bit
	 * Frame format: MSB First
	 */
	spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
		SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	nvic_enable_irq(NVIC_SPI2_IRQ); // enable SPI interrupt
}

/**
 * GPIO initialisaion: clocking + pins setup
 */
void GPIO_init(){
/*	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_SPI1);
	rcc_periph_clock_enable(RCC_GPIOC);*/
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN |
			RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN |
			RCC_APB2ENR_IOPEEN);
	// USB_DISC: push-pull
	gpio_set_mode(USB_DISC_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, USB_DISC_PIN);
	// USB_POWER: open drain, externall pull down with R7 (22k)
	gpio_set_mode(USB_POWER_PORT, GPIO_MODE_INPUT,
				GPIO_CNF_INPUT_FLOAT, USB_POWER_PIN);
	// AD7794 addr + en
	gpio_set_mode(ADC_ADDR_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, ADC_ADDR_MASK | ADC_EN_PIN); // ADDRESS: PD10..12; EN: PD13
	gpio_clear(ADC_ADDR_PORT, ADC_ADDR_MASK | ADC_EN_PIN); // clear address & turn switch off
}

/*
 * SysTick used for system timer with period of 1ms
 */
void SysTick_init(){
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // Systyck: 72/8=9MHz
	systick_set_reload(8999); // 9000 pulses: 1kHz
	systick_interrupt_enable();
	systick_counter_enable();
}

/*
 * Due to inconvenient pins position on STM32F103VxT6 I had to make this strange location:
 * my channel #   ->   ADC1/2 channel #
 * 0 -> 9    PB1
 * 1 -> 8    PB0
 * 2 -> 15   PC5
 * 3 -> 14   PC4
 * 4 -> 7    PA7
 * 5 -> 6    PA6
 * 6 -> 5    PA5
 * 7 -> 4    PA4
 */
uint8_t adc_channel_array[16] = {9,8,15,14,7,6,5,4};
#define ADC_CHANNELS_NUMBER    8
void ADC_init(){
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_ADC1EN); // enable clocking
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV4);
	rcc_periph_clock_enable(RCC_GPIOA | RCC_GPIOB | RCC_GPIOC); // clocking for ADC ports
	// channels 4-7: PA7-PA4
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO4|GPIO5|GPIO6|GPIO7);
	// channels 0,1: PB1, PB0
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0|GPIO1);
	// channels 2,3: PC5, PC4
	gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO4|GPIO5);

	// Make sure the ADC doesn't run during config
	adc_off(ADC1);

	// first configure DMA1 Channel1 (ADC1)
	rcc_periph_clock_enable(RCC_DMA1); // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	dma_channel_reset(DMA1, DMA_CHANNEL1); //DMA_DeInit(DMA1_Channel1);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t) &(ADC_DR(ADC1))); // DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) ADC_value); // DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_value;
	dma_set_number_of_data(DMA1, DMA_CHANNEL1, ADC_CHANNELS_NUMBER); // DMA_InitStructure.DMA_BufferSize = 1;
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1); // DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1); // DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL1); // DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT); // DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT); // DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dma_enable_circular_mode(DMA1, DMA_CHANNEL1); // DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	dma_set_priority(DMA1, DMA_CHANNEL1, DMA_CCR_PL_HIGH); // DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	dma_enable_channel(DMA1, DMA_CHANNEL1); // DMA_Cmd(DMA1_Channel1, ENABLE);

	// Configure ADC as continuous scan mode with DMA
	adc_set_dual_mode(ADC_CR1_DUALMOD_IND); // ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	adc_enable_scan_mode(ADC1); // ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	adc_set_continuous_conversion_mode(ADC1); // ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	adc_disable_external_trigger_regular(ADC1); // ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adc_set_right_aligned(ADC1); // ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5CYC); // ADC_SampleTime_239Cycles5
	//adc_set_sample_time(ADC1, ADC_CHANNEL8, ADC_SMPR_SMP_239DOT5CYC); // ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);
	adc_enable_dma(ADC1); // ADC_DMACmd(ADC1, ENABLE);
	adc_power_on(ADC1); // ADC_Cmd(ADC1, ENABLE);
}

/**
 * Starts ADC calibration & after it runs ADC in continuous conversion mode
 * First call ADC_init(), than wait a little and call this function
 */
void ADC_calibrate_and_start(){
	adc_set_regular_sequence(ADC1, ADC_CHANNELS_NUMBER, adc_channel_array);
	adc_reset_calibration(ADC1);
	adc_calibration(ADC1);
	adc_start_conversion_regular(ADC1); // ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	adc_start_conversion_direct(ADC1);
}

uint16_t tim2_buff[8] = {10,20,30,40,50,60,70,80};
uint16_t tim2_inbuff[8];

void init_dmatimer(){ // tim2_ch4 - PA3, no remap
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, GPIO3);
	rcc_periph_clock_enable(RCC_TIM2);
	timer_reset(TIM2);
	// timers have frequency of 1MHz -- 1us for one step
	// 36MHz of APB1
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	// 72MHz div 72 = 1MHz
	timer_set_prescaler(TIM2, 71); // prescaler is (div - 1)
	timer_continuous_mode(TIM2); // automatically reload
	timer_enable_preload(TIM2); // force changing period
	timer_set_period(TIM2, 86); // period is 87us
	timer_enable_update_event(TIM2);
	timer_set_oc_mode(TIM2, TIM_OC4, TIM_OCM_PWM1); // edge-aligned mode
	timer_enable_oc_preload(TIM2, TIM_OC4);
	timer_enable_oc_output(TIM2, TIM_OC4);
	timer_enable_irq(TIM2, TIM_DIER_UDE | TIM_DIER_CC4DE | TIM_DIER_CC3DE);
	timer_set_oc_polarity_low(TIM2, TIM_OC4);

	// TIM2_CH4 - DMA1, channel 7
	rcc_periph_clock_enable(RCC_DMA1);
	dma_channel_reset(DMA1, DMA_CHANNEL7);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL7, (uint32_t) &(TIM_CCR4(TIM2)));
	dma_set_read_from_memory(DMA1, DMA_CHANNEL7);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL7);
	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL7);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL7, DMA_CCR_PSIZE_16BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL7, DMA_CCR_MSIZE_16BIT);
	dma_enable_transfer_error_interrupt(DMA1, DMA_CHANNEL7);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL7);
	dma_set_memory_address(DMA1, DMA_CHANNEL7, (uint32_t)tim2_buff);

	nvic_enable_irq(NVIC_DMA1_CHANNEL7_IRQ); // enable dma1_channel7_isr

	// capture: TIM2_CH3
	timer_ic_set_input(TIM2, TIM_IC3, TIM_IC_IN_TI4);
	timer_set_oc_polarity_high(TIM2, TIM_OC3);
	timer_ic_enable(TIM2, TIM_IC3);
	timer_enable_oc_output(TIM2, TIM_OC3);

	// configure DMA1 Channel1 (ADC1)
	dma_channel_reset(DMA1, DMA_CHANNEL1);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t) &(TIM_CCR3(TIM2)));
	dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) tim2_inbuff);
	dma_set_number_of_data(DMA1, DMA_CHANNEL1, 8);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL1);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
	nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);
	dma_enable_transfer_error_interrupt(DMA1, DMA_CHANNEL1);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);
}

void run_dmatimer(){
	int i;
	DMA1_IFCR = DMA_ISR_TEIF7|DMA_ISR_HTIF7|DMA_ISR_TCIF7|DMA_ISR_GIF7 |
		DMA_ISR_TEIF1|DMA_ISR_HTIF1|DMA_ISR_TCIF1|DMA_ISR_GIF1; // clear flags

	dma_set_number_of_data(DMA1, DMA_CHANNEL7, 8);
	timer_set_dma_on_compare_event(TIM2);
	timer_enable_oc_output(TIM2, TIM_OC4);
	dma_enable_channel(DMA1, DMA_CHANNEL7);
	timer_enable_counter(TIM2);

	for(i = 0; i < 8; i++) tim2_inbuff[i] = 0;
	dma_set_number_of_data(DMA1, DMA_CHANNEL1, 8);
	dma_enable_channel(DMA1, DMA_CHANNEL1);
}

void dma1_channel7_isr(){
	if(DMA1_ISR & DMA_ISR_TCIF7) {
		DMA1_IFCR = DMA_IFCR_CTCIF7;
		dma_disable_channel(DMA1, DMA_CHANNEL7);
	}else if(DMA1_ISR & DMA_ISR_TEIF7){
		DMA1_IFCR = DMA_IFCR_CTEIF7;
		MSG("out transfer error\n");
	}
}

void dma1_channel1_isr(){
	int i;
	if(DMA1_ISR & DMA_ISR_TCIF1) {
		DMA1_IFCR = DMA_IFCR_CTCIF1;
		dma_disable_channel(DMA1, DMA_CHANNEL1);
		timer_disable_counter(TIM2);
		gpio_set(GPIOA, GPIO3);
		for(i = 0; i < 8; i++){
			print_int(tim2_inbuff[i], lastsendfun);
			MSG(" ");
		}
		MSG("\n");
	}else if(DMA1_ISR & DMA_ISR_TEIF1){
		DMA1_IFCR = DMA_IFCR_CTEIF1;
		MSG("in transfer error\n");
	}
}

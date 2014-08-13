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

#include "main.h"
#include "hardware_ini.h"

volatile uint16_t ADC_value[8]; // ADC DMA value

/**
 * GPIO initialisaion: clocking + ports setup
 */
void GPIO_init(){
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, GPIO11|GPIO12); // LED + USB
	// AD7794 addr + en
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, ADC_ADDR_MASK | GPIO10); // ADDRESS: PC6..9; EN: PC10
	GPIO_BSRR(GPIOC) = (ADC_ADDR_MASK | GPIO10) << 16; // clear address & disable com
}

void SysTick_init(){
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // Systyck: 72/8=9MHz
	systick_set_reload(8999); // 9000 pulses: 1kHz
	systick_interrupt_enable();
	systick_counter_enable();
}

void ADC_init(){
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_ADC1EN); // enable clocking
	rcc_periph_clock_enable(RCC_ADC1);
	rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV4);
	rcc_periph_clock_enable(RCC_GPIOB | RCC_GPIOC); // clocking for ADC ports
	gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, 3); // ADC8 - PB0, ADC9 -PB1
	gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, 63); // ADC10-15: PC0-PC5

	// Make sure the ADC doesn't run during config
	adc_off(ADC1);

	// first configure DMA1 Channel1 (ADC1)
	rcc_periph_clock_enable(RCC_DMA1); // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	dma_channel_reset(DMA1, DMA_CHANNEL1); //DMA_DeInit(DMA1_Channel1);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t) &(ADC_DR(ADC1))); // DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) ADC_value); // DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_value;
	dma_set_number_of_data(DMA1, DMA_CHANNEL1, 8); // DMA_InitStructure.DMA_BufferSize = 1;
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
	//adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_239DOT5CYC); // ADC_SampleTime_239Cycles5
	adc_set_sample_time(ADC1, ADC_CHANNEL8, ADC_SMPR_SMP_239DOT5CYC); // ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);
	adc_enable_dma(ADC1); // ADC_DMACmd(ADC1, ENABLE);
	adc_power_on(ADC1); // ADC_Cmd(ADC1, ENABLE);
}

/**
 * Starts ADC calibration & after it runs ADC in continuous conversion mode
 * First call ADC_init(), than wait a little and call this function
 */
void ADC_calibrate_and_start(){
	uint8_t channel_array[16] = {8,9,10,11,12,13,14,15};
	// adc_set_regular_sequence 1 channel -- 0 // ADC_InitStructure.ADC_NbrOfChannel = 1;
	channel_array[0] = ADC_CHANNEL8;
	adc_set_regular_sequence(ADC1, 8, channel_array);
	adc_reset_calibration(ADC1);
	adc_calibration(ADC1);
	adc_start_conversion_regular(ADC1); // ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	adc_start_conversion_direct(ADC1);
}


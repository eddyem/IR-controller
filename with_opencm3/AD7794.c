/*
 * AD7794.c - routines to work with ADC AD7794 by SPI
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

#include "main.h"
#include "spi.h"
#include "cdcacm.h"


volatile uint8_t data_error = 0;
uint8_t SPI_buffer[4]; // IO buffer: contains data to transmit & received data

#define check_err()  if(data_error){DBG("SPI error!\n"); data_error = 0; return 0;}
#define ERR_SPI() do{data_error = 1; return NULL;}while(0)

void SPI_buffer_clear(){
	uint8_t i;
	for(i = 0; i < 4; i++)
		SPI_buffer[i] = 0;
}

/**
 * Send len bytes from SPI buffer to register reg
 * @param reg - register to write + key READ_FROM_REG in case of read request
 * @param len - length of data to transmit/receive (without reg!): max 3!!!
 * @return received data
 */
uint8_t *send_SPI_data(uint8_t reg, uint8_t len){
	data_error = 0;
	if(len > 3) return NULL; // error: too big data array
	SPI_buffer[0] = reg;
	len++;
	if(!write_SPI(SPI_buffer, len))
		ERR_SPI();
	if(!read_SPI(SPI_buffer, len))
		ERR_SPI();
	return &SPI_buffer[1];
}

#undef ERR_SPI
#define ERR_SPI() do{data_error = 1; return 0;}while(0)
/**
 * Write data to 8bit register over SPI
 * @param reg  - register to write
 * @param data - data to write
 * @return readed data
 */
uint8_t sendByte(uint8_t reg, uint8_t data){
	SPI_buffer_clear();
	SPI_buffer[1] = data;
	if(!send_SPI_data(reg, 1))
		ERR_SPI();
	return SPI_buffer[1]; // return readed data
}
/**
 * Write data to 16bit register over SPI
 * @param reg  - register to write
 * @param data - data to write
 * @return readed data
 */
uint16_t sendWord(uint8_t reg, uint16_t data){
	uint16_t ret;
	SPI_buffer_clear();
	SPI_buffer[1] = (data >> 8 )& 0xff; // HSB first
	SPI_buffer[2] = data & 0xff;
	if(!send_SPI_data(reg, 2))
		ERR_SPI();
	ret = ((uint16_t)SPI_buffer[1]) << 8 | SPI_buffer[2];
	return ret; // return readed data
}
/**
 * reads 24 bit ADC data register
 * @return value of ADC || 1 (0 indicates error)
 */
uint32_t read_ADC_data(){
	uint32_t ret;
	SPI_buffer_clear();
	if(!send_SPI_data(DATA_REGISTER | READ_FROM_REG, 3))
		return 0;
	ret = SPI_buffer[1] << 16 | SPI_buffer[2] << 8 | SPI_buffer[3];
	if(!ret) ret = 1; // there could be a very little case that we get 0
	return ret;
}

/**
 * Checks ending of ADC and/or errors
 * @return 1 in case of data ready, set data_error to 1 in case of error
 */
uint8_t check_data_ready(){
	uint8_t ret = 0;
	//DBG("check_data_ready\r\n");
	uint8_t x = sendByte(STAT_REGISTER | READ_FROM_REG, 0);
	if(data_error){
		DBG("some data error\n");
		ret = 1; // data not ready, but error flag is set
	}else if((x & 0x18) != 0x08 || x & DATA_ERROR || x & STATUS_NOREF){ // test for AD7794 status register
		#ifdef EBUG
		if((x & 0x18) != 0x08) DBG("There's no AD7794 signature found\n");
		if(x & DATA_ERROR) DBG("DATA_ERROR\n");
		if(x & STATUS_NOREF) DBG("STATUS_NOREF\n");
		#endif
		if(x) data_error = x;
		else data_error = 1; // 0 -> no signature
		ret = 1;
	}else if(!(x & DATA_NOTRDY)) ret = 1;
	return ret;
}

#define read_AD7794_conf()		sendWord(CONF_REGISTER | READ_FROM_REG, 0)

static uint8_t ADC_gain = 1;
/**
 * Changes ADC gain coefficient
 * @param gain - log2(gain)
 * gain	voltage range (Vref+-)
 * 0	2.5 V
 * 1	1.25 V
 * 2	625 mV
 * 3	312.5 mV
 * 4	156.2 mV
 * 5	78.125 mV
 * 6	39.06 mV
 * 7	19.53 mV
 * @return 0 in case of wrong gain value
 */
uint8_t change_AD7794_gain(uint8_t gain){
	if(gain > 7) return 0;
	ADC_gain = gain;
	uint16_t Conf = read_AD7794_conf();
	check_err();
	Conf &= ~GAIN_MASK; // clear gain bits
	Conf |=  ADC_gain << 8; // update gain
	sendWord(CONF_REGISTER, Conf);
	check_err();
	return 1;
}

uint8_t AD7794_set_channel(uint8_t channel){
	uint16_t Conf = read_AD7794_conf();
	check_err();
	Conf &= ~CHANNEL_MASK; // clear channel number
	Conf |= (CHANNEL_MASK & channel); // set new channel
	sendWord(CONF_REGISTER, Conf);
	check_err();
	return 1;
}

/**
 * make internal full-scale calibration for given channel
 * !! first run setup_AD7794() and after - this function !!
 * @param channel - channel to activate
 * @return 0 on error
 */
uint8_t AD7794_calibration(uint8_t channel){
	if(ADC_gain == 7){
		DBG("gain = 7, can't calibrate!\n");
		return 0;
	}
	if(!AD7794_set_channel(channel)) return 0;
	sendWord(MODE_REGISTER,
				INT_FS_CAL | U16(0x0f)); // make a full-scale calibration on lowest speed
	check_err();
	return 1;
}

/**
 * Setup ADC: write keys to registers
 * @param config - keys to be written to configuration register
 * @param io     - keys to be written to io register
 * @return 0 on error
 */
uint8_t setup_AD7794(uint16_t config, uint8_t io){
	sendWord(MODE_REGISTER, POWERDOWN_MODE); // put ADC into powerdown mode
	sendWord(CONF_REGISTER,
				ADC_gain << 8 // gain
				| config);
	check_err();
	sendByte(IO_REGISTER, io);
	check_err();
	return 1;
}

// we modify this macro to zero steps counter in case of error
#define check_errR()  if(data_error){DBG("SPI error!\n"); data_error = 0; N = 0; return 0;}
/**
 * Start ADC transform & read data value
 * !! first run setup_AD7794(), AD7794_calibration() and after - this function !!
 * @param channel - channel number to transform
 * @return:
 * 		AD7794_NOTRDY when measurement in process
 * 		ADC data value if ready
 * 		0 in case of error
 */
uint32_t read_AD7794(uint8_t channel){
	static uint8_t N = 0; // operation number
	uint8_t dr;
	switch (N){
		case 0: // start: set channel
			if(!AD7794_set_channel(channel)){
				return 0; // 0 in return means error
			}
		break;
		case 1: // put ADC to a single conversion mode
			sendWord(MODE_REGISTER, SINGLE_MODE |
				U16(0x0f)); // the lowest speed;
			check_errR();
		break;
		case 2: // wait for data reading & check errors
			dr = check_data_ready();
			check_errR();
			if(!dr) return AD7794_NOTRDY;
		break;
		default: // last step -> return readed data
			N = 0;
			return read_ADC_data();
	}
	N++;
	return AD7794_NOTRDY;
}

/**
 * This function tries to reset ADC's SPI controller & check returned status
 * If
 * @param
 * @return !!! ADC_NO_ERROR in case of succsess!!!
 * 			Other error code in case of error (ADC_ERR_NO_DEVICE is critical error!)
 */
uint8_t reset_AD7794(){
	uint8_t i, ret = ADC_NO_ERROR;
	for(i = 0; i < 3; i++){
		sendByte(0xFF, 0xFF); // >= 32 bits of 1
		if(data_error) break;
	}
	if(data_error){
		DBG("SPI error: can't send reset sequence!\n");
		data_error = 0;
		return ADC_ERR_NO_DEVICE;
	}
	Delay(1); // give some time to "reboot"
	check_data_ready();
	if(data_error){
		DBG("SPI error: device can't do reset!\n");
		if((data_error & 0x18) != 0x08) // no signature
			ret = ADC_ERR_NO_DEVICE;
		else
			ret = ADC_ERR_OTHER;
		data_error = 0;
	}
	return ret;
}



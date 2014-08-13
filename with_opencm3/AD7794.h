/*
 * AD7794.h
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
#pragma once
#ifndef __AD7794_H__
#define __AD7794_H__

#include "main.h"

// in function read_AD7794 zero returning means error, so we should do something
// to detect "data not ready yet"; 24 bits of ADC allow us to use some
// big values as flags
#define AD7794_NOTRDY	((uint32_t)0xffffffff)

uint8_t setup_AD7794(uint16_t config, uint8_t io);
uint8_t AD7794_calibration(uint8_t channel);
uint32_t read_AD7794(uint8_t channel);
uint8_t reset_AD7794();
uint8_t change_AD7794_gain(uint8_t gain);

// error codes of reset_AD7794()
enum {
	ADC_NO_ERROR,
	ADC_ERR_NO_DEVICE,
	ADC_ERR_OTHER
};

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * in case of communication error set DIN high and give to SCLK >=32 pulses
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

/*
 * Communication register
 * | CR7 | CR6 | CR5 | CR4 | CR3 | CR2 | CR1 | CR0 |
 *  !WEN   R/!W  RS2   RS1   RS0  CREAD   0     0
 *
 * - WEN   (write enable)  : 1 - ignore all bits in packet
 * - R/!W  (next operation): 0 - write, 1 - read
 * - RS2..RS0 (reg choice) :
 * 		(RS2RS1RS0)
 * 		000: CR - write / SR - read (8 bit)
 * 		001: Mode register  (16 bit)
 * 		010: Configuration register  (16 bit)
 * 		011: Data register (24 bit)
 * 		100: ID register - read only (8 bit)
 * 		101: IO register (8 bit)
 * 		110: Offset register (24 bit)
 * 		111: Full-Scale register(24 bit)
 * - CREAD (continious read): 1 - enable continious reading (put 01011100 to
 * 			CR & take DIN in zero state
 * 			to reset this mode write to CR 01011000 or make ADC reset
 * CR1&CR0 always should be 0!
 */
// masks for CR:
#define NOT_WEN			U8(0x80)
#define WRITE_TO_REG	U8(0)
#define READ_FROM_REG	U8(0x40)
#define CONT_READ		U8(0x04)
#define STAT_REGISTER	U8(0)
#define MODE_REGISTER	U8(0x08)
#define CONF_REGISTER	U8(0x10)
#define DATA_REGISTER	U8(0x18)
#define ID_REGISTER		U8(0x20)
#define IO_REGISTER		U8(0x28)
#define	OFFSET_REGISTER	U8(0x30)
#define FS_REGISTER		U8(0x38)
#define REGISTER_MASK	U8(0x38)	// (REGISTER_MASK & CR) >> 3 gives register number

/*
 * Status register
 * | SR7 | SR6 | SR5 | SR4 | SR3 | SR2 | SR1 | SR0 |
 *  !RDY  ERR   NOXREF  0     1    CH2   CH1   CH0
 *
 * - !RDY (data ready)      : ==0 when data is ready for reading
 * - ERR (ADC error flag)   : ==1 in case of some error  (cleared by hardware in ADC starting)
 * - NOXREF (no ref voltage): ==1 in cese of REFIN1/REFIN2 less than threshold
 * SR4&SR3 are always 01
 * - CH2..CH0 (channel number): number of current channel
 */
// masks for SR:
#define DATA_NOTRDY			U8(0x80)
#define DATA_ERROR			U8(0x40)
#define STATUS_NOREF		U8(0x20)
#define DATA_CHANNEL_MASK	U8(0x07)	// Channel selection: DATA_CHANNEL_MASK & ch. number

/*
 * Mode register (16 bit)
 * | MRf | MRe | MRd | MRc | MRb | MRa | MR9 | MR8 | MR7 | MR6 | MR5 | MR4 | MR3 | MR2 | MR1 | MR0 |
 *   MD2   MD1   MD0   PSW    0     0   AMP-CM  0   CLK1  CLK0    0  CHOP-DIS FS3  FS2   FS1   FS0
 *
 * - MD2..MD0 (mode)    : mode selection
 * 						000 - continuous mode
 * 						001 - single mode
 * 						010 - idle mode
 * 						011 - power-down mode
 * 						100 - internal zero-scale calibration
 * 						101 - internal full-scale calibration
 * 						110 - system zero-scale calibration
 * 						111 - system full-scale calibration
 * - PSW (PSW mode)     : ==1 to disconnect PSW and ground, ==0 to connect (!!! limit current is 30mA !!!)
 * - AMP-CM (amplify)   : ==0 to wide amplification (badly disables synphase signal)
 * - CLK1, CLK0 (CLCsrc): CLC source
 * 							00 - inner 64kHz, CLK out is off
 * 							01 - inner 64kHz, CLK out is on
 * 							10 - outer 64kHz connected to CLK in
 * 							11 - outer (freq/2) connected to CLK in
 * CHOP-DIS (syn dis)   : used to disable synphase signal, sets together with AMP-CM
 * FS3..FS0 (fltr per.) : period of filter update rate
 */
// masks for MR:
// modes:
#define MODE_MASK		U16(0xe000)
#define CONT_MODE		U16(0)
#define SINGLE_MODE		U16(0x2000)
#define IDLE_MODE		U16(0x4000)
#define POWERDOWN_MODE	U16(0x6000)
#define INT_ZS_CAL		U16(0x8000)
#define INT_FS_CAL		U16(0xa000)
#define SYS_ZS_CAL		U16(0xc000)
#define SYS_FS_CAL		U16(0xe000)
// other
#define PSW_DISCONN		U16(0x1000)
#define AMPCM_RESET		U16(0x0200)
// clocking
#define CLK_IN_NOOUT	U16(0)
#define CLK_IN_SYNC		U16(0x0040)
#define CLK_OUT_64		U16(0x0080)
#define CLK_OUT_DIV2	U16(0x00c0)
// other
#define CHOP_DISABLE	U16(0x0010)
#define FILTER_MASK		U16(0x000f)

/*
 * Configuration register
 * | CRf | CRe | CRd | CRc | CRb | CRa | CR9 | CR8 | CR7 | CR6 | CR5 | CR4 | CR3 | CR2 | CR1 | CR0 |
 *   VBIAS1/0     BO   U/!B  BOOST  G2   G1     G0   REFSEL1/0  REF_DET BUF   CH3..0
 *
 * - VBIAS1/0 (enable bias voltage generator):
 * 							00 - VBIAS disabled
 * 							01 - connect to AIN1
 * 							10 - connect to AIN2
 * 							11 - connect to AIN3
 * - BO (burnout current)  : enable 100nA current source (buffer or in-amp should be active)
 * - U/!B (uni/bipolar)    : ==1 for unipolar coding (full-scale from 0x000000 to 0xffffff)
 * 						     ==0 for bipolar coding (0x800000 is zero, > - positive)
 * - BOOST (increase curnt): ==1 to increase inner VREF current consuming to reduce power-up time
 * - G2..G0 (gain)         : select ADC ampl. gain, GAIN = 1<<G, input ranges are
 * 			2.5V, 1.25V, 625mV, 312.5mV, 156.25mV, 78.125mV, 39.06mV, 19.53mV
 * - REFSEL1/0 (ref.select): reference voltage
 * 							00 - external between REFIN1(+) and REFIN1(-)
 * 							01 - external between REFIN2(+) and REFIN2(-)
 * 							10 - internal 1.17V
 * 							11 - reserved
 * - REF_DET (detect ref)  : enable reference detection by set/reset bit NOXREF
 * - BUF (buffered mode)   : configure ADC for buffered (1) mode  (==1 automatically for gain > 2)
 * - CH3..0 (chnnl select) : select channel (reserved are non-shown)
 * 							     [CHANNEL] - [Calibration pair]
 * 							0000 - AIN1 - 0
 * 							0001 - AIN2 - 1
 * 							0010 - AIN3 - 2
 * 							0011 - AIN4 - 3
 * 							0100 - AIN5 - 3
 * 							0101 - AIN6 - 3
 * 							0110 - Temp sensor  - int. 1.17VREF + gain=1
 * 							0111 - ADVV monitor - int. 1.17VREF + gain=1/6
 * 							1000 - AIN1(-)/AIN1(-) - 0
 */
// masks for SR
// vbias source
#define VBIAS_1			U16(0x4000)
#define VBIAS_2			U16(0x8000)
#define VBIAS_3			U16(0xc000)
// other
#define INT_CURRENT_SRC	U16(0x2000)
#define UNIPOLAR_CODING	U16(0x1000)
#define BOOST_UP		U16(0x0800)
// gain mask
#define GAIN_MASK		U16(0x0700) // gain == 1<<G
// refin selection
#define EXTREFIN_1		U16(0)
#define EXTREFIN_2		U16(0x0040)
#define INTREFIN		U16(0x0080)
// other
#define REF_DETECTION	U16(0x0020)
#define BUFFERED_OP		U16(0x0010)
// channel selection mask
#define CHANNEL_MASK	U16(0x000f) // CHANNEL_MASK & (channel number - 1)
#define INR_TEMP_SENSOR	U16(0x0006) // inner thermometer
#define AVDD_MONITOR	U16(0x0007) // monitor avdd
#define CHANNEL1_MINUS	U16(0x0008) // AIN1(-)/AIN1(-)

/*
 * Data register stores the conversion result: 24bits
 * on completion reading !RDY is set
 */

/*
 * ID register is read-only register storing identification number of ADC: 8bits
 */

/*
 * IO register
 * | IO7 | IO6 | IO5 | IO4 | IO3 | IO2 | IO1 | IO0 |
 *    0   IOEN     IODAT      IEXCDIR     IEXCEN
 *
 * - IOEN (digital out)   : ==1 to configure AIN6 as digital output pins P1/P2
 * - IODAT (P2/P1 data)   : when IOEN==1 this is a data for digital ports
 * - IEXCDIR (cur.src dir): direction of currens sources
 * 						00 - IEXC1 connected to IOUT1, IEXC2 connected to IOUT2
 * 						01 - IEXC1 connected to IOUT2, IEXC2 connected to IOUT1
 * 						10 - both sources connected to IOUT1 (only for 10/210uA)
 * 						11 - both sources connected to IOUT2 (-//-)
 * - IEXCEN (en. cur.src) : enable/disable current sources
 * 						00 - currents are disabled
 * 						01 - 10uA
 * 						10 - 210uA
 * 						11 - 1mA
 */
// masks for IOR
#define IOEN			U8(0x40)
#define IODAT_MASK		U8(0x30)
#define IEXC_DIRECT		U8(0)
#define IEXC_SWAPPED	U8(0x04)
#define IEXC_BOTH1		U8(0x08)
#define IEXC_BOTH2		U8(0x0c)
#define IEXC_DISABLE	U8(0)
#define IEXC_10UA		U8(0x01)
#define IEXC_210UA		U8(0x02)
#define IEXC_1MA		U8(0x03)

/*
 * Offset register holds the offset calibration coefficient (R/W, 24bit)
 * Each calibration channel has its own offset registry
 * Value automatically overwtitten on calibration
 * To directly write into that registry ADC should be placed in power-down or idle mode
 */

/*
 * Full-scale register (R/W, 24 bit)
 * It holds the full-scale calibration coefficients fir ADC
 * Each calibration channel has its own full-scale registry
 * To directly write into that registry ADC should be placed in power-down or idle mode
 */
#endif // __AD7794_H__

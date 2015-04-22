/*
 * usb_prop.h
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
#ifndef __USB_PROP_H__
#define __USB_PROP_H__

#include "user_protocol.h" // for flags

typedef struct{
	uint32_t bitrate;  // Data terminal rate, in bits per second
	uint8_t format;    // Stop bits: 0 - 1SB, 1 - 1.5SB, 2 - 2SB
	uint8_t paritytype;// Parity: 0-none, 1-odd, 2-even, 3-mark, 4-space
	uint8_t datatype;  // Data bits (5, 6, 7, 8 or 16)
}LINE_CODING;

extern LINE_CODING linecoding;


// by default all user functions are NULL
// comment that, which are used
#define SomeDev_GetConfiguration          NOP_Process
// #define SomeDev_SetConfiguration          NOP_Process
#define SomeDev_GetInterface              NOP_Process
#define SomeDev_SetInterface              NOP_Process
#define SomeDev_GetStatus                 NOP_Process
#define SomeDev_ClearFeature              NOP_Process
#define SomeDev_SetEndPointFeature        NOP_Process
#define SomeDev_SetDeviceFeature          NOP_Process
//#define SomeDev_SetDeviceAddress          NOP_Process

// commands through EP0
#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

void SomeDev_init(void);
void SomeDev_Reset(void);
void SomeDev_SetConfiguration(void);
void SomeDev_ClearFeature(void);
void SomeDev_SetDeviceAddress (void);
void SomeDev_Status_In (void);
void SomeDev_Status_Out (void);
RESULT SomeDev_Data_Setup(uint8_t);
RESULT SomeDev_NoData_Setup(uint8_t);
RESULT SomeDev_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *SomeDev_GetDeviceDescriptor(uint16_t );
uint8_t *SomeDev_GetConfigDescriptor(uint16_t);
uint8_t *SomeDev_GetStringDescriptor(uint16_t);
uint8_t *SomeDev_GetLineCoding(uint16_t Length);
uint8_t *SomeDev_SetLineCoding(uint16_t Length);

#endif // __USB_PROP_H__

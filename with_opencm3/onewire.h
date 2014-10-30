/*
 * onewire.h
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
#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include "main.h"
#include "hardware_ini.h"

// 1-wire status
#define OW_OK			(1)
#define OW_ERROR		(2)
#define OW_NO_DEVICE	(3)

#define OW_NO_READ		(0xff)

#define OW_READ_SLOT	(uint8_t*)"0xff"

void OW_Init();
uint8_t OW_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen);
uint8_t OW_Get(uint8_t buflen, uint8_t *data, uint8_t readStart);
uint8_t OW_Scan(uint8_t *buf, uint8_t num);

// shortcuts for functions
// only send message b wich length is c with RESET flag a
#define OW_SendOnly(a,b,c)  OW_Send(a, b, c)
// send 1 command (with bus reset)
#define OW_WriteCmd(cmd) OW_Send(1, cmd, 1)
// send 1 function (without bus reset)
#define OW_WriteFn(cmd) OW_Send(0, cmd, 1)
#define OW_Wait_TX() while(!(USART_SR(OW_USART_X) & USART_SR_TC))

void OW_getTemp();

/*
 * thermometer identificator is: 8bits CRC, 48bits serial, 8bits device code (10h)
 * Critical temperatures is T_H and T_L
 * T_L is lowest allowed temperature
 * T_H is highest -//-
 * format T_H and T_L: 1bit sigh + 7bits of data
 */

/*
 * thermometer commands (DS18S20)\
 * send them with bus reset!
 */
// find devices
#define T_SEARCH_ROM		(0xf0)
#define OW_SEARCH_ROM		(uint8_t*)"\xf0"
// read device (when it is alone on the bus)
#define T_READ_ROM			(0x33)
#define OW_READ_ROM			(uint8_t*)"\x33"
// send device ID (after this command - 8 bytes of ID)
#define T_MATCH_ROM			(0x55)
#define OW_MATCH_ROM		(uint8_t*)"\x55"
// broadcast command
#define T_SKIP_ROM			(0xcc)
#define OW_SKIP_ROM			(uint8_t*)"\xcc"
// find devices with critical conditions
#define T_ALARM_SEARCH		(0xec)
#define OW_ALARM_SEARCH		(uint8_t*)"\xec"
/*
 * thermometer functions
 * send them without bus reset!
 */
// start themperature reading
#define T_CONVERT_T			(0x44)
#define OW_CONVERT_T		(uint8_t*)"\x44"
// write critical temperature to device's RAM
#define T_SCRATCHPAD		(0x4e)
#define OW_SCRATCHPAD		(uint8_t*)"\x4e"
// read whole device flash
#define T_READ_SCRATCHPAD	(0xbe)
#define OW_READ_SCRATCHPAD	(uint8_t*)"\xbe"
// copy critical themperature from device's RAM to its EEPROM
#define T_COPY_SCRATCHPAD	(0x48)
#define OW_COPY_SCRATCHPAD	(uint8_t*)"\x48"
// copy critical themperature from EEPROM to RAM (when power on this operation runs automatically)
#define T_RECALL_E2			(0xb8)
#define OW_RECALL_E2		(uint8_t*)"\xb8"
// check whether there is devices wich power up from bus
#define T_READ_POWER_SUPPLY (0xb4)
#define OW_READ_POWER_SUPPLY (uint8_t*)"\xb4"


/*
 * RAM register:
 * 0 - themperature: 1 ADU == 0.5 degrC
 * 1 - sign (0 - T>0 degrC ==> T=byte0; 1 - T<0 degrC ==> T=byte0-0xff+1)
 * 2 - T_H
 * 3 - T_L
 * 4 - 0xff (reserved)
 * 5 - 0xff (reserved)
 * 6 - COUNT_REMAIN (0x0c)
 * 7 - COUNT PER DEGR (0x10)
 * 8 - CRC
 */


#endif // __ONEWIRE_H__

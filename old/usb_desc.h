/*
 * usb_desc.h
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
#ifndef __USB_DESC_H__
#define __USB_DESC_H__

#include "stm32f10x.h"

// descriptors' sizes
#define SomeDev_SIZ_DEVICE_DESC      18
#define SomeDev_SIZ_CONFIG_DESC      67
#define SomeDev_SIZ_STRING_LANGID    4
#define SomeDev_SIZ_STRING_VENDOR    42
#define SomeDev_SIZ_STRING_PRODUCT   58
#define SomeDev_SIZ_STRING_SERIAL    18
#define SomeDev_SIZ_STRING_INTERFACE 30
#define SomeDev_SIZ_STRINGS          4

// interrupts & data buffers size
#define SomeDev_INT_SIZE              8
#define SomeDev_DATA_SIZE             64


// exported data
extern const uint8_t SomeDev_DeviceDescriptor[SomeDev_SIZ_DEVICE_DESC];
extern const uint8_t SomeDev_ConfigDescriptor[SomeDev_SIZ_CONFIG_DESC];
extern const uint8_t SomeDev_StringLangID[SomeDev_SIZ_STRING_LANGID];
extern const uint8_t SomeDev_StringVendor[SomeDev_SIZ_STRING_VENDOR];
extern const uint8_t SomeDev_StringProduct[SomeDev_SIZ_STRING_PRODUCT];
extern const uint8_t SomeDev_StringSerial[SomeDev_SIZ_STRING_SERIAL];
extern const uint8_t SomeDev_StringInterface[SomeDev_SIZ_STRING_INTERFACE];

#endif // __USB_DESC_H__

/*
 * usb_desc.c - descriptors of USB device
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

#include "usb_desc.h"

/*
 * device configuration:
 * 1 configuration, 1 interface
 * 4 bulk-type endpoints
 */
const uint8_t SomeDev_DeviceDescriptor[SomeDev_SIZ_DEVICE_DESC] = {
	0x12, // bLength
	0x01, // bDescriptorType
	0x00,
	0x02, // bcdUSB = 2.00
	0x02, // bDeviceClass: CDC
	0x00, // bDeviceSubClass
	0x00, // bDeviceProtocol
	0x40, // bMaxPacketSize
	0x83,
	0x04, // idVendor = 0x0483
	0x40,
	0x57, // idProduct = 0x5740
	0x00,
	0x02, // bcdDevice = 2.00
	1,    // Index of string descriptor describing manufacturer
	2,    // Index of string descriptor describing product
	3,    // Index of string descriptor describing the device's serial number
	1     // bNumConfigurations
};

const uint8_t SomeDev_ConfigDescriptor[SomeDev_SIZ_CONFIG_DESC] = {
	// configuration:
	0x09, // bLength: Configuration Descriptor size
	0x02, // bDescriptorType: Configuration
	SomeDev_SIZ_CONFIG_DESC, // wTotalLength: no of returned bytes
	0x00,
	0x02, // bNumInterfaces: 2 interface
	0x01, // bConfigurationValue: Configuration value
	0x00, // iConfiguration: Index of string descriptor describing the configuration
	0xC0, // bmAttributes: self powered
	0x32, // MaxPower 0 mA
	// interface:
	0x09, // bLength: Interface Descriptor size
	0x04, // bDescriptorType: Interface
	0x00, // bInterfaceNumber: Number of Interface
	0x00, // bAlternateSetting: Alternate setting
	0x01, // bNumEndpoints: One endpoint used
	0x02, // bInterfaceClass: Communication Interface Class
	0x02, // bInterfaceSubClass: Abstract Control Model
	0x01, // bInterfaceProtocol: Common AT commands
	0x00, // iInterface
	// header functional descriptor:
	0x05, // bLength: Endpoint Descriptor size
	0x24, // bDescriptorType: CS_INTERFACE
	0x00, // bDescriptorSubtype: Header Func Desc
	0x10, // bcdCDC: spec release number
	0x01,
	// call management functional descriptor:
	0x05, // bFunctionLength
	0x24, // bDescriptorType: CS_INTERFACE
	0x01, // bDescriptorSubtype: Call Management Func Desc
	0x00, // bmCapabilities: D0+D1
	0x01, // bDataInterface: 1
	// ACM Functional Descriptor:
	0x04, // bFunctionLength
	0x24, // bDescriptorType: CS_INTERFACE
	0x02, // bDescriptorSubtype: Abstract Control Management desc
	0x02, // bmCapabilities
	// Union Functional Descriptor:
	0x05, // bFunctionLength
	0x24, // bDescriptorType: CS_INTERFACE
	0x06, // bDescriptorSubtype: Union func desc
	0x00, // bMasterInterface: Communication class interface
	0x01, // bSlaveInterface0: Data Class Interface
	// Endpoint 2 Descriptor:
	0x07, // bLength: Endpoint Descriptor size
	0x05, // bDescriptorType: Endpoint
	0x82, // bEndpointAddress: (IN2)
	0x03, // bmAttributes: Interrupt
	SomeDev_INT_SIZE, // wMaxPacketSize: interrupt command
	0x00,
	0xFF, // bInterval
	// Data class interface descriptor
	0x09, // bLength: Endpoint Descriptor size
	0x04, // bDescriptorType
	0x01, // bInterfaceNumber: Number of Interface
	0x00, // bAlternateSetting: Alternate setting
	0x02, // bNumEndpoints: Two endpoints used
	0x0A, // bInterfaceClass: CDC
	0x00, // bInterfaceSubClass
	0x00, // bInterfaceProtocol
	0x00, // iInterface
	// Endpoint 3 Descriptor
	0x07, // bLength: Endpoint Descriptor size
	0x05, // bDescriptorType: Endpoint
	0x03, // bEndpointAddress: (OUT3)
	0x02, // bmAttributes: Bulk
	SomeDev_DATA_SIZE, // wMaxPacketSize: data out
	0x00,
	0x00, // bInterval: ignore for Bulk transfer
	// Endpoint 1 Descriptor
	0x07, // bLength: Endpoint Descriptor size
	0x05, // bDescriptorType: Endpoint
	0x81, // bEndpointAddress: (IN1)
	0x02, // bmAttributes: Bulk
	SomeDev_DATA_SIZE,  // wMaxPacketSize: data in
	0x00,
	0x00  // bInterval
};


/*
 * String identifiers
 * 		this ID's are data array sent to PC by request
 * 		as they are just strings, they could be generated dinamically
 */

// Language
const uint8_t SomeDev_StringLangID[SomeDev_SIZ_STRING_LANGID] = {
	SomeDev_SIZ_STRING_LANGID,
	0x03,
	0x09,
	0x04
}; // LangID = 0x0409: U.S. English

// Vendor
const uint8_t SomeDev_StringVendor[SomeDev_SIZ_STRING_VENDOR] = {
	SomeDev_SIZ_STRING_VENDOR, // Size of manufaturer string
	0x03, // bDescriptorType = String descriptor
	// Manufacturer:
	'S', 0,
	'A', 0,
	'O', 0,
	' ', 0,
	'R', 0,
	'A', 0,
	'S', 0,
	',', 0,
	' ', 0,
	'S', 0,
	'u', 0,
	'p', 0,
	'p', 0,
	'l', 0,
	'y', 0,
	' ', 0,
	'L', 0,
	'a', 0,
	'b', 0,
	' ', 0
};

// Product
const uint8_t SomeDev_StringProduct[SomeDev_SIZ_STRING_PRODUCT] = {
	SomeDev_SIZ_STRING_PRODUCT,
	0x03,
	// Product name:
	'S', 0,
	'T', 0,
	'M', 0,
	'3', 0,
	'2', 0,
	' ', 0,
	'U', 0,
	'S', 0,
	'B', 0,
	'/', 0,
	'C', 0,
	'O', 0,
	'M', 0,
	' ', 0,
	'c', 0,
	'o', 0,
	'n', 0,
	't', 0,
	'r', 0,
	'o', 0,
	'l', 0,
	' ', 0,
	'M', 0,
	'o', 0,
	'd', 0,
	'u', 0,
	'l', 0,
	'e', 0
};

// Serial
const uint8_t SomeDev_StringSerial[SomeDev_SIZ_STRING_SERIAL] = {
	SomeDev_SIZ_STRING_SERIAL,
	0x03,
	// Serial number
	'A', 0,
	'p', 0,
	'p', 0,
	' ', 0,
	'V', 0,
	'0', 0,
	'0', 0,
	'1', 0
};


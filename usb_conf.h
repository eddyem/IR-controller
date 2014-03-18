/*
 * usb_conf.h - base endpoints address and callbacks
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
#ifndef __USB_CONF_H__
#define __USB_CONF_H__

/*
 * EP_NUM defines how many endpoints are used by the device
 */
#define EP_NUM                          (4)


// buffer table base address
#define BTABLE_ADDRESS      (0x00)
// EP0 - command rx/tx buffer base address
#define ENDP0_RXADDR        (0x40)
#define ENDP0_TXADDR        (0x80)
// EP1 - data (unused) tx buffer base address
#define ENDP1_TXADDR        (0xC0)
// EP2 & EP3 - data tx/rx buffer base address
#define ENDP2_TXADDR        (0x100)
#define ENDP3_RXADDR        (0x110)

/*
 * mask defining which events has to be handled
 * by the device application software:
 *
 * CNTR_CTRM   - correct transfer interrupt
 * CNTR_DOVRM  - DMA over/underrun
 * CNTR_ERRM   - USB error
 * CNTR_WKUPM  - wakeup request
 * CNTR_SUSPM  - suspend request
 * CNTR_RESETM - USB reset request
 * CNTR_SOFM   - SOF (start of frame) packets
 * CNTR_ESOFM  - expected start of frame packet
 */
#define IMR_MSK (CNTR_CTRM  | CNTR_SOFM  | CNTR_RESETM)


// clear all unused endpoints' callbacks
//#define  EP1_IN_Callback   NOP_Process
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

#define  EP1_OUT_Callback   NOP_Process
#define  EP2_OUT_Callback   NOP_Process
//#define  EP3_OUT_Callback  NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process


#endif // __USB_CONF_H__

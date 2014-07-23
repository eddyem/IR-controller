/*
 * uart.h
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

#pragma once
#ifndef __UART_H__
#define __UART_H__

// Size of buffers
#define UART_TX_DATA_SIZE            64

void UART_init(uint32_t UART);
void UART_setspeed(uint32_t UART, struct usb_cdc_line_coding *linecoding);

void uart1_send(uint8_t byte);
void uart2_send(uint8_t byte);
void uart3_send(uint8_t byte);


#endif // __UART_H__

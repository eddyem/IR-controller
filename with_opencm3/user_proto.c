/*
 * user_proto.c
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

#include "cdcacm.h"
#include "main.h"
#include "uart.h"
#include "hardware_ini.h"

// integer value given by user
static volatile int32_t User_value = 0;
// flag: !=0 when user value reading ends - for character terminals
enum{
	UVAL_START,		// user start to write integer value
	UVAL_CHECKED,	// user checks the value & enters + that it's right
	UVAL_PRINTED,	// value printed to user
	UVAL_ENTERED,	// value entered but not printed
	UVAL_BAD		// entered bad value
};
uint8_t Uval_ready = UVAL_PRINTED;

int read_int(char *buf, int cnt);

intfun I = NULL; // function to process entered integer

#define READINT() do{i += read_int(&buf[i+1], len-i-1);}while(0)
#define WRONG_COMMAND() do{command = '?';}while(0)

void parce_incoming_buf(char *buf, int len, sendfun s){
	uint8_t command;
	int i = 0;
	if(Uval_ready == UVAL_START){ // we are in process of user's value reading
		i += read_int(buf, len);
	}
	if(Uval_ready == UVAL_ENTERED){
		print_int(User_value, s); // printout readed integer value for error control
		Uval_ready = UVAL_PRINTED;
	}
	if(I && Uval_ready == UVAL_CHECKED){
		Uval_ready = UVAL_BAD; // clear Uval_ready
		I(User_value, s);
	}
	for(; i < len; i++){
		command = buf[i];
		if(!command) continue; // omit zero
		switch (command){
			case 'A': // show ADC value
				//adc_start_conversion_direct(ADC1);
				P("\r\n ADC value: ", s);
				print_int(ADC_value, s);
				newline(s);
			break;
			case 'b': // turn LED off
				gpio_set(GPIOC, GPIO12);
			break;
			case 'B': // turn LED on
				gpio_clear(GPIOC, GPIO12);
			break;
			case 'I': // read & print integer value
				I = process_int;
				READINT();
			break;
			case '+': // user check number value & confirm it's right
				if(Uval_ready == UVAL_PRINTED) Uval_ready = UVAL_CHECKED;
				else WRONG_COMMAND();
			break;
			case '-': // user check number value & confirm it's wrong
				if(Uval_ready == UVAL_PRINTED) Uval_ready = UVAL_BAD;
				else WRONG_COMMAND();
			break;
			case 'u': // check USB connection
				P("\r\nUSB ", s);
				if(!USB_connected) P("dis", s);
				P("connected\r\n",s);
			break;
/*
			case 'U': // test: init USART1
				UART_init(USART1);
			break;
*/
			case '\n': // show newline as is
			break;
			case '\r':
			break;
			default:
				WRONG_COMMAND(); // echo '?' on unknown command
		}
		s(command); // echo readed byte
	}
}

/*
 * Send char array wrd thru USB or UART
 */
void prnt(uint8_t *wrd, sendfun s){
	if(!wrd) return;
	while(*wrd) s(*wrd++);
}

void newline(sendfun s){
	P("\r\n", s);
}

// sign of readed value
int32_t sign;
/**
 * Read from TTY integer value given by user (in DEC).
 *    Reading stops on first non-numeric symbol.
 *    To work with symbol terminals reading don't stops on buffer's end,
 *    it waits for first non-numeric char.
 *    When working on string terminals, terminate string by '\n', 0 or any other symbol
 * @param buf - buffer to read from
 * @param cnt - buffer length
 * @return amount of readed symbols
 */
int read_int(char *buf, int cnt){
	int readed = 0, i;
	if(Uval_ready){ // this is first run
		Uval_ready = UVAL_START; // clear flag
		User_value = 0; // clear value
		sign = 1; // clear sign
	}
	if(!cnt) return 0;
	for(i = 0; i < cnt; i++, readed++){
		uint8_t chr = buf[i];
		if(chr == '-'){
			if(sign == 1){
				sign = -1;
				continue;
			}else{ // '-' after numbers
				Uval_ready = UVAL_ENTERED;
				break;
			}
		}
		if(chr < '0' || chr > '9'){
			Uval_ready = UVAL_ENTERED;
			break;
		}
		User_value = User_value * 10 + (int32_t)(chr - '0');
	}
	if(Uval_ready != UVAL_START) // reading has met an non-numeric character
		User_value *= sign;
	return readed;
}

/**
 * Print decimal integer value
 * @param N - value to print
 * @param s - function to send a byte
 */
void print_int(int32_t N, sendfun s){
	uint8_t buf[10], L = 0;
	if(N < 0){
		s('-');
		N = -N;
	}
	if(N){
		while(N){
			buf[L++] = N % 10 + '0';
			N /= 10;
		}
		while(L--) s(buf[L]);
	}else s('0');
}

void process_int(int32_t v, sendfun s){
	newline(s);
	P("You have entered a value ", s);
	print_int(v, s);
	newline(s);
}

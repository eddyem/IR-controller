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
// number of motor to process
static volatile uint8_t active_motor = 6;
// last active send function - to post "anonymous" replies
sendfun lastsendfun = usb_send; // make it usb_send by default (to prevent suspension)
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

/**
 * displays all TRD values
 * @param s -- current output
 */
void print_ad_vals(sendfun s){
	int j;
	if(ad7794_on){
		for(j = 0; j < TRD_NO; j++){
			print_int(ad7794_values[j], s);
			s(' ');
		}
		newline(s);
	}else{
		P("no AD7794 found\n", s);
	}
}

void set_shtr_delay(int32_t v, sendfun s){
	uint16_t d = (uint16_t) v;
	Shutter_delay = d;
	P("Change delay to ", s);
	print_int(d, s);
}

void parce_incoming_buf(char *buf, int len, sendfun s){
	uint8_t command;
	int i = 0, j, m;
	lastsendfun = s;
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
		if(command >= '0' && command <= '4'){ // stepper motors
			active_motor = command - '0';
			I = stepper_proc;
			READINT();
		}else switch (command){
			case 'P':
				OW_fill_ID(0);
				//run_dmatimer();
			break;
			case 'x': // set period of TIM1 (motors 1..3)
				active_motor = 1;
				I = set_timr;
				READINT();
			break;
			case 'X': // set period of TIM2 (motors 4,5)
				active_motor = 4;
				I = set_timr;
				READINT();
			break;
			case 'B': // stop all motors
				for(m = 0; m < 5; m++)
					stop_motor(m);
			break;
		/*	case 'W': // scan for one 1-wire device
				if(1 == OW_Scan(onewire_addr, 1)){
					P("found 1-wire: ", s);
					print_hex(onewire_addr, 8, s);
				}else
					P("1-wire error",s );
				P("\n", s);
			break;*/
			case 'S': // single conversion
				doubleconv = 0;
			break;
			case 'D': // double conversion
				doubleconv = 1;
			break;
			case 'A': // show TRD values
				//adc_start_conversion_direct(ADC1);
				P("\nTRD resistance: ", s);
				for(j = 0; j < 8; j++){
					print_int(TRD_value(i), s);
					s('\t');
				}
				newline(s);
			break;
			case 'i': // init AD7794
				AD7794_init();
			break;
			case 'I': // turn off flag AD7794
				ad7794_on = 0;
			break;
			case '+': // user check number value & confirm it's right
				if(Uval_ready == UVAL_PRINTED) Uval_ready = UVAL_CHECKED;
				else WRONG_COMMAND();
			break;
			case '-': // user check number value & confirm it's wrong
				if(Uval_ready == UVAL_PRINTED) Uval_ready = UVAL_BAD;
				else WRONG_COMMAND();
			break;
			case 'g': // change gain
				I = set_ADC_gain;
				READINT();
			break;
			case 's': // read ADC val through SPI
				if(!ad7794_on){
					AD7794_init();
					P("wait: values aren't ready yet\n", s);
					break;
				}
				P("AD7794 values: ", s);
				print_ad_vals(s);
			break;
			case 'u': // check USB connection
				P("\nUSB ", s);
				if(!USB_connected) P("dis", s);
				P("connected\n",s);
			break;
			case 'M': // ADC monitoring ON
				ADC_monitoring = !ADC_monitoring;
			break;
			case 'T': // print time
				print_time(s);
				newline(s);
			break;
		/*	case 'U': // test: init USART1
				UART_init(USART1);
			break; */
			case 'p': // show motors voltage * 100
				print_int(power_voltage(), s);
				newline(s);
			break;
			case 'h': // show sHutter voltage * 100
				print_int(shutter_voltage(), s);
				newline(s);
			break;
			case 'r': // reinit shutter
				shutter_init();
			break;
			case 't': // print shutter state
				print_shutter_state(s);
			break;
			case 'c': // close shutter
				try_to_close_shutter();
			break;
			case 'o': // open shutter
				try_to_open_shutter();
			break;
			case 'F': // dump flash data
				dump_flash_data(s);
			break;
			case 'd': // change ADC_divisor
				;
			break;
			case 'm': // change ADC_multiplier
				;
			break;
			case 'z': // temporary: refresh
				flash_status = check_flash_data();
				print_int(flash_status, s);
				s('\n');
			break;
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

/*
void newline(sendfun s){
	P("\n", s);
}
*/

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
 * Print buff as hex values
 * @param buf - buffer to print
 * @param l   - buf length
 * @param s   - function to send a byte
 */
void print_hex(uint8_t *buff, uint8_t l, sendfun s){
	void putc(uint8_t c){
		if(c < 10)
			s(c + '0');
		else
			s(c + 'a' - 10);
	}
	s('0'); s('x'); // prefix 0x
	while(l--){
		putc(buff[l] >> 4);
		putc(buff[l] & 0x0f);
	}
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

/*
void process_int(int32_t v, sendfun s){
	newline(s);
	P("You have entered a value ", s);
	print_int(v, s);
	newline(s);
}
*/

void set_ADC_gain(int32_t v, sendfun s){
	if(ad7794_on){
		P("Change gain to ", s);
		print_int(v, s);
		newline(s);
		change_AD7794_gain(v);
		AD7794_calibration(0);
	}
}

/**
 * Process stepper
 * @param v - user value
 * @param s - active sendfunction
 */
void stepper_proc(int32_t v, sendfun s){
	if(active_motor > 4){
		P("wrong motor number\n", s);
		return; // error
	}
	move_motor(active_motor, v);
	active_motor = 6;
}

void set_timr(int32_t v, sendfun s){
	if(active_motor > 4){
		P("wrong motor number\n", s);
		return; // error
	}
	if(v < 0 || v > 0xffff){
		MSG("Bad period!\n");
		active_motor = 6;
		return;
	}
	MSG("set period: ");
	print_int(v, lastsendfun);
	MSG("\n");
	set_motor_period(active_motor, (uint16_t)v);
	active_motor = 6;
}

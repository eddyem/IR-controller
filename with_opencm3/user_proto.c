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
#include "flash.h"

// mode:
curmode_t mode = BYTE_MODE; // text protocol, activated on 1st meeteng of '['
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
#define WRONG_COMMAND() do{if(mode == BYTE_MODE) command = '?';}while(0)

/**
 * displays all TRD values from AD7794
 * @param s -- current output
 */
void print_ad_vals(sendfun s){
	int j;
	if(ad7794_on){
		for(j = 0; j < TRD_NO; j++){
			if(mode == LINE_MODE){
				P("[ " STR_EXTADC_VALUES " ", s);
				print_int(j, s);
				s(' ');
			}
			print_int(ad7794_values[j], s);
			if(mode == LINE_MODE)
				P(" ]\n", s);
			else s(' ');
		}
		if(mode == BYTE_MODE) newline(s);
	}else{
		if(mode == BYTE_MODE) P("no AD7794 found\n", s);
		else if(mode == LINE_MODE) P("[ " STR_EXTADC_VALUES " ERR ]\n", s);
	}
}

/**
 * displays all TRD values from inner ADC
 * @param s -- current output
 */
void print_int_ad_vals(sendfun s){
	int j;
	for(j = 0; j < 8; j++){
		if(mode == LINE_MODE){
			P("[ " STR_INTADC_VALUES " ", s);
			print_int(j, s);
			s(' ');
		}
		print_int(TRD_value(j), s);
		if(mode == BYTE_MODE) s(' ');
		else if(mode == LINE_MODE)
			P(" ]\n", s);
	}
}

/*
uint8_t set_shtr_delay(int32_t v, sendfun s){
	uint16_t d = (uint16_t) v;
	Shutter_delay = d;
	P("Change delay to ", s);
	print_int(d, s);
	return 0;
}
*/

int adc_channel = -1;
int div_mul = 0; // 0 - multip., !0 - div.
/**
 * change dividers/multipliers
 * work only in BYTE_MODE
 */
uint8_t ch_divmul(int32_t v, sendfun s){
	uint32_t val = (uint32_t) v;
	if(adc_channel == -1) return 0;
	if(div_mul){ // != 0 - divisors
		flash_store_U32((uint32_t)&ADC_divisors[adc_channel], &val);
	}else{ // == 0 - mul
		flash_store_U32((uint32_t)&ADC_multipliers[adc_channel], &val);
	}
	adc_channel = -1;
	P("stored\n", s);
	return 1;
}
/**
 * Change divisor (work only in BYTE_MODE)
 * @param v - user value (sensor number)
 * @param s - active sendfunction
 */
uint8_t try_ch_divmul(int32_t v, sendfun s){
	if(v > ADC_CHANNELS_NUMBER || v < 0){
		P("wrong channel number\n", s);
		adc_channel = -1;
		return 0; // error
	}
	adc_channel = v;
	I = ch_divmul;
	read_int(NULL, 0); //start reading next int
	return 0;
}

/**
 * prints state of end switches for motor v
 */
uint8_t endswitchstate(int32_t v, sendfun s){
	int32_t i;
	if(v < 0 || v > 4){
		if(mode == BYTE_MODE) P("Wrong motor number\n", s);
		return 0;
	}
	i = check_ep(v);
	if(mode == BYTE_MODE){
		P("Endswitches for motor ", s);
		s('0' + v);
		P(": ", s);
	}else if(mode == LINE_MODE){ // in LINE_MODE print [ E No_motor state ]
		P("[ " STR_ENDSW_STATE " ",s);
		s('0' + v);
		s(' ');
	}
	if(mode != BINARY_MODE){
		print_int(i, s);
		if(mode == LINE_MODE) P(" ]\n", s);
		else s('\n');
	}
	return 0;
}

/**
 * moves turret 'active_motor' to position v
 */
uint8_t move_turret(int32_t v, sendfun s){
	const int32_t maxpos[3] = {8, 6, 6}; // maximum position number for given turret
	int32_t sign = 1;
	if(active_motor > 2){
		if(mode == BYTE_MODE) P("Wrong turret number\n", s);
		return 0;
	}
	int32_t m = maxpos[active_motor];
	if(v > m){
		if(mode == BYTE_MODE){
			P("Wrong position, shoud be not more than ", s);
			print_int(m, s);
		}
		return 0;
	}
	move2pos[active_motor] = v;
	// check the nearest direction
	uint8_t curpos = check_ep(active_motor);
	if(curpos){ // we are not between positions
		if((v + m - curpos) > m/2) // rotation in positive direction will take more steps than in negative
			sign = -1; // move CCV
	}
	return move_motor(active_motor, TURRETS_NEXT_POS_STEPS * sign);
}

/**
 * function 'help' works only in BYTE_MODE
 */
void help(sendfun s){
	#define pr(arg)   do{prnt((uint8_t*)arg, s); s('\n');}while(0)
	pr("0..4\t move stepper motor to N steps");
	pr("Commands in char protocol:");
	pr(STR_INTADC_VALUES "\tget TRD values");
	pr(STR_STOP_ALL_MOTORS "\tstop all motors");
	//pr("C");
	pr("D\tturn AD7794 to double conversion mode");
	pr(STR_ENDSW_STATE "\tshow end-switches state for given motor");
	pr("F\tdump flash data");
	//pr("G");
	pr("H\tshow this help");
	pr("I\tturn off AD7794 init flag");
	pr("J\tmove slits (0) wheel to Nth position");
	pr("K\tmove filters (1) wheel to Nth position");
	pr("L\tmove filters (2) wheel to Nth position");
	pr("M\tswitch on/off ADC monitoring");
	//pr("N");
	//pr("O");
	pr("P\ttest (ow_fill_id)");
	//pr("Q");
	//pr("R");
	pr("S\tturn AD7794 to single conversion mode");
	pr(STR_PRINT_TIME "\tprint current value of time counters");
	//pr("U\t(reserved)");
	//pr("V");
	//pr("W\t(reserved)");
	pr("X\tset timer period for linear stages");
	//pr("Y");
	//pr("Z");
	//pr("a");
	//pr("b");
	pr("c\tclose shutter");
	pr("d\tchange value of ADC divisor No N");
	//pr("e");
	//pr("f");
	pr("g\tchange AD7794 gain");
	pr(STR_SHTR_VOLTAGE "\tshow shutter voltage");
	pr(STR_EXTADC_INIT "\tinit AD7794");
	//pr("j");
	//pr("k");
	//pr("l");
	pr("m\tchange value of ADC multiplier No N");
	//pr("n");
	pr("o\topen shutter");
	pr(STR_MOTORS_VOLTAGE "\tshow motors voltage");
	//pr("q");
	pr("r\treinit shutter");
	pr(STR_EXTADC_VALUES "\tshow AD7794 values");
	pr(STR_SHTR_STATE "\tprint shutter state");
	pr("u\tcheck USB connection");
	//pr("v");
	//pr("w");
	pr("x\tset timer period for turrets' motors");
	//pr("y");
	//pr("z");
	pr("\nAfter entering of user value press '+' to submit or '-' to reject it");
	#undef pr
}

/**
 * parce command buffer buf with length len
 * return 0 if buffer processed or len if there's not enough data in buffer
 */
int parce_incoming_buf(char *buf, int len, sendfun s){
	uint8_t command, do_echo = 1;
	//uint32_t utmp;
	int i = 0, j, m;
	lastsendfun = s;
	if(buf[0] == '[') mode = LINE_MODE;
	if(mode == BYTE_MODE){
		if(Uval_ready == UVAL_START){ // we are in process of user's value reading
			i += read_int(buf, len);
		}
		if(Uval_ready == UVAL_ENTERED){
			P("confirm entered value (+/-): ", s);
			print_int(User_value, s); // printout readed integer value for error control
			Uval_ready = UVAL_PRINTED;
		}
		if(I && Uval_ready == UVAL_CHECKED){
			Uval_ready = UVAL_BAD; // clear Uval_ready
			I(User_value, s);
			return 0;
		}
	}else if(mode == LINE_MODE){ // text mode: check for "]\n" presence
		uint8_t bad_cmd = 1;
s(buf[0]);
P(" check buffer in line mode: ", s);
		if(buf[0] == '['){
		for(j = 1; j < len; j++){
s(buf[j]);
			if(buf[j] != '\n') continue; // search end of line
			else{
				if(buf[j-1] == ']'){
P("OK, good!\n", s);
					bad_cmd = 0;
					len = j; buf[j] = 0; // truncate buffer to only one command

					break;
				}
				else{
P("broken command!\n",s);
					return 0; // end of line without closing bracket
				}
			}
		}} else{
P("wrong first\n",s );
			return 0;
		}
		if(bad_cmd){
P("not full\n",s);
			return len; // not enough data in buffer
		}
	}
	for(; i < len; i++){
		command = buf[i];
		if(!command) continue; // omit zero
		if(command >= '0' && command <= '4'){ // stepper motors
			active_motor = command - '0';
			I = stepper_proc;
			READINT();
		}else switch (command){
			case '[': // line mode - do nothing, this is start symbol
				//mode = LINE_MODE;
			break;
			case '+': // user check number value & confirm it's right
				if(mode != BYTE_MODE) return 0; // bad command - no echo
				if(Uval_ready == UVAL_PRINTED) Uval_ready = UVAL_CHECKED;
				else WRONG_COMMAND();
			break;
			case '-': // user check number value & confirm it's wrong
				if(mode != BYTE_MODE) return 0; // bad command - no echo
				if(Uval_ready == UVAL_PRINTED) Uval_ready = UVAL_BAD;
				else WRONG_COMMAND();
			break;
			case CMD_INTADC_VALUES: // [A] show TRD values by inner ADC [ A 0 val0 ]\n[A 1 val1 ] ...
				//adc_start_conversion_direct(ADC1);
				print_int_ad_vals(s);
				if(mode == BYTE_MODE) s('\n');
				do_echo = 0; // don't echo command in line mode as there's output
			break;
			case 'b': // byte mode
				mode = BYTE_MODE;
				//return 0; //don't echo this command
			break;
			case CMD_STOP_ALL_MOTORS: // [B] stop all motors
				for(m = 0; m < 5; m++)
					stop_motor(m);
				do_echo = 0; // echo called from sto
			break;
			case 'c': // [c] close shutter
				do_echo = try_to_close_shutter(); // don't echo in case of state wouldn't be changed
			break;
			case 'd': // change ADC_divisor (only for byte mode)
				if(mode != BYTE_MODE) return 0;
				div_mul = 1; //divisors
				I = try_ch_divmul;
				READINT();
			break;
			case 'D': // double conversion (only for byte mode)
				if(mode != BYTE_MODE) return 0;
				doubleconv = 1;
			break;
			case CMD_ENDSW_STATE: // [E] print end-switch state for given motor
				I = endswitchstate;
				READINT();
			break;
			case 'F': // dump flash data (only in byte mode)
				if(mode != BYTE_MODE) return 0;
				dump_flash_data(s);
			break;
			case 'g': // change gain (only in byte mode)
				if(mode != BYTE_MODE) return 0;
				I = set_ADC_gain;
				READINT();
			break;
			case CMD_SHTR_VOLTAGE: // [h] show sHutter voltage * 100
				if(mode == LINE_MODE) P("[ " STR_SHTR_VOLTAGE " ", s);
				print_int(shutter_voltage(), s);
				if(mode == LINE_MODE) P(" ]", s);
				newline(s);
				do_echo = 0;
			break;
			case 'H': // show help (only in byte mode)
				if(mode != BYTE_MODE) return 0;
				help(s);
			break;
			case CMD_EXTADC_INIT: // [i] init AD7794
				AD7794_init();
			break;
			case 'I': // turn off flag AD7794
				ad7794_on = 0;
			break;
			case 'J': // move turret 0/1/2 to given position
			case 'K':
			case 'L':
				active_motor = command - 'J'; // J == 0, K == 1, L == 2
				I = move_turret;
				READINT();
			break;
			case 'm': // change ADC_multiplier (only for byte mode)
				if(mode != BYTE_MODE) return 0;
				div_mul = 0; // multipliers
				I = try_ch_divmul;
				READINT();
			break;
			case 'M': // ADC monitoring ON
				ADC_monitoring = !ADC_monitoring;
			break;
			case 'o': // open shutter
				do_echo = try_to_open_shutter();
			break;
			case CMD_MOTORS_VOLTAGE: // [p] show motors voltage * 100
				if(mode == LINE_MODE) P("[ " STR_MOTORS_VOLTAGE " ", s);
				print_int(power_voltage(), s);
				if(mode == LINE_MODE) P(" ]", s);
				newline(s);
			break;
			case 'P': //  (only for byte mode)
				if(mode != BYTE_MODE) return 0;
				OW_fill_ID(0);
				//run_dmatimer();
			break;
			case 'r': // reinit shutter
				shutter_init();
			break;
			case CMD_EXTADC_VALUES: // [s] read ADC val through SPI
				if(!ad7794_on){
					AD7794_init();
					if(mode == BYTE_MODE) P("wait: values aren't ready yet\n", s);
					break;
				}
				if(mode == BYTE_MODE) P("AD7794 values: ", s);
				print_ad_vals(s);
				do_echo = 0;
			break;
			case 'S': // single conversion (only for byte mode)
				if(mode != BYTE_MODE) return 0;
				doubleconv = 0;
			break;
			case CMD_SHTR_STATE: // [t] print shutter state
				print_shutter_state(s);
				do_echo = 0;
			break;
			case CMD_PRINT_TIME: // print time
				print_time(s);
				if(mode == BYTE_MODE) newline(s);
			break;
			case 'u': // check USB connection (only byte mode)
				if(mode != BYTE_MODE) return 0;
				P("\nUSB ", s);
				if(!USB_connected) P("dis", s);
				P("connected\n",s);
			break;
		/*	case 'U': // test: init USART1
				UART_init(USART1);
			break; */
		/*	case 'W': // scan for one 1-wire device
				if(1 == OW_Scan(onewire_addr, 1)){
					P("found 1-wire: ", s);
					print_hex(onewire_addr, 8, s);
				}else
					P("1-wire error",s );
				P("\n", s);
			break;*/
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
			case '\n': // show newline, space and tab as is
			case '\r':
			case ' ':
			case '\t':
			break;
			default:
				WRONG_COMMAND(); // echo '?' on unknown command in byte mode
		}
		if(mode == BYTE_MODE) s(command); // echo readed byte in byte mode
	}
	if(mode == LINE_MODE){ // process command which needs for user value
		if(I && Uval_ready == UVAL_ENTERED){
			Uval_ready = UVAL_BAD; // clear Uval_ready
			do_echo = I(User_value, s);
			I = NULL;
		}
	}
	if(do_echo && mode == LINE_MODE)
		P(buf, s);
	return 0; // all data processed - 0 bytes leave in buffer
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
	static int enteredDigits; // amount of entered digits
	static int sign; // sign of readed value
	if(Uval_ready){ // this is first run
		Uval_ready = UVAL_START; // clear flag
		enteredDigits = 0; // 0 digits entered
		User_value = 0; // clear value
		sign = 1; // clear sign
	}
	if(!cnt) return 0;
	for(i = 0; i < cnt; i++, readed++){
		uint8_t chr = buf[i];
		if(chr == '-'){
			if(enteredDigits == 0){ // sign should be first
				sign = -1;
				continue;
			}else{ // '-' after number - reject entered value
				Uval_ready = UVAL_BAD;
				break;
			}
		}
		if(chr < '0' || chr > '9'){
			if(enteredDigits)
				Uval_ready = UVAL_ENTERED;
			else{
				if(chr == ' ' || chr == '\t') continue; // omit leading space|tab
				else Uval_ready = UVAL_BAD; // bad symbol
			}
			break;
		}
		User_value = User_value * 10 + (int32_t)(chr - '0');
		enteredDigits++;
	}
	if(Uval_ready == UVAL_ENTERED) // reading has met an non-numeric character
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

/**
 * change AD7794 gain. Run only in byte mode!
 */
uint8_t set_ADC_gain(int32_t v, sendfun s){
	if(ad7794_on){
		P("Change gain to ", s);
		print_int(v, s);
		newline(s);
		change_AD7794_gain(v);
		AD7794_calibration(0);
	}
	return 0;
}

/**
 * Process stepper
 * @param v - user value
 * @param s - active sendfunction
 */
uint8_t stepper_proc(int32_t v, sendfun s){
	if(active_motor > 4){
		if(mode == BYTE_MODE) P("wrong motor number\n", s);
		return 0; // error
	}
	uint8_t ret = move_motor(active_motor, v);
	active_motor = 6;
	return ret;
}

uint8_t set_timr(int32_t v, sendfun s){
	if(active_motor > 4){
		if(mode == BYTE_MODE) P("wrong motor number\n", s);
		return 0; // error
	}
	if(v < 0 || v > 0xffff){
		if(mode == BYTE_MODE) P("Bad period!\n", s);
		active_motor = 6;
		return 0;
	}
	if(mode == BYTE_MODE){
		P("set period: ", s);
		print_int(v, s);
		P("\n", s);
	}
	set_motor_period(active_motor, (uint16_t)v);
	active_motor = 6;
	return 1;
}

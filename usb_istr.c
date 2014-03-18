/*
 * usb_istr.c - callback for command endpoints
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

#include "includes.h"

__IO uint16_t wIstr;  // ISTR register last read value
__IO uint8_t bIntPackSOF = 0; // SOFs received between 2 consecutive packets

void (*pEpInt_IN[7])(void) = {
	EP1_IN_Callback,
	EP2_IN_Callback,
	EP3_IN_Callback,
	EP4_IN_Callback,
	EP5_IN_Callback,
	EP6_IN_Callback,
	EP7_IN_Callback,
};

void (*pEpInt_OUT[7])(void) = {
	EP1_OUT_Callback,
	EP2_OUT_Callback,
	EP3_OUT_Callback,
	EP4_OUT_Callback,
	EP5_OUT_Callback,
	EP6_OUT_Callback,
	EP7_OUT_Callback,
};


// command interrupts; IMR_MSK defined in usb_conf.h
void USB_Istr(void){
	wIstr = _GetISTR();
// correct transfer interrupt
#if (IMR_MSK & ISTR_CTR)
	if (wIstr & ISTR_CTR & wInterrupt_Mask) CTR_LP();
#endif
// USB reset request
#if (IMR_MSK & ISTR_RESET)
	if (wIstr & ISTR_RESET & wInterrupt_Mask){
		_SetISTR((uint16_t)CLR_RESET);
		Device_Property.Reset();
	}
#endif
// DMA over/underrun
#if (IMR_MSK & ISTR_DOVR)
	if (wIstr & ISTR_DOVR & wInterrupt_Mask) _SetISTR((uint16_t)CLR_DOVR);
#endif
// USB error
#if (IMR_MSK & ISTR_ERR)
  if (wIstr & ISTR_ERR & wInterrupt_Mask) _SetISTR((uint16_t)CLR_ERR);
#endif
// wakeup request
#if (IMR_MSK & ISTR_WKUP)
	if (wIstr & ISTR_WKUP & wInterrupt_Mask){
		_SetISTR((uint16_t)CLR_WKUP);
		Resume(RESUME_EXTERNAL);
	}
#endif
// suspend request
#if (IMR_MSK & ISTR_SUSP)
	if (wIstr & ISTR_SUSP & wInterrupt_Mask){
		// check if SUSPEND is possible
		if (fSuspendEnabled) Suspend();
		else // if not possible then resume after xx ms
			Resume(RESUME_LATER);
		// clear of the ISTR bit must be done after setting of CNTR_FSUSP
		_SetISTR((uint16_t)CLR_SUSP);
	}
#endif
// SOF (start of frame) packets
#if (IMR_MSK & ISTR_SOF)
	if (wIstr & ISTR_SOF & wInterrupt_Mask){
		_SetISTR((uint16_t)CLR_SOF);
		bIntPackSOF++;
		SOF_Callback();
	}
#endif
// expected start of frame packet
#if (IMR_MSK & ISTR_ESOF)
	if (wIstr & ISTR_ESOF & wInterrupt_Mask){
		_SetISTR((uint16_t)CLR_ESOF);
		// resume handling timing is made with ESOFs
		Resume(RESUME_ESOF); // request without change of the machine state
	}
#endif
} /* USB_Istr */


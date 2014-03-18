/*
 * usb_pwr.c - USB power (on/off, suspend, resume)
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

#include "usb_pwr.h"

__IO uint32_t bDeviceState = UNCONNECTED; /* USB device status */
__IO bool fSuspendEnabled = FALSE;  /* true when suspend is possible */

struct{
	__IO RESUME_STATE eState;
	__IO uint8_t bESOFcnt;
} ResumeS;

RESULT PowerOn(void){
	uint16_t wRegVal;
	USB_Cable_Config(DISABLE);
	/*** CNTR_PWDN = 0 ***/
	wRegVal = CNTR_FRES;
	_SetCNTR(wRegVal);
	/*** CNTR_FRES = 0 ***/
	wInterrupt_Mask = 0;
	_SetCNTR(wInterrupt_Mask);
	/*** Clear pending interrupts ***/
	_SetISTR(0);
	/*** Set interrupt mask ***/
	wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
	_SetCNTR(wInterrupt_Mask);
	/*** cable plugged-in ? ***/
	/*while(!CablePluggedIn());*/
	USB_Cable_Config(ENABLE);
	return USB_SUCCESS;
}

RESULT PowerOff(){
	/* disable all interrupts and force USB reset */
	_SetCNTR(CNTR_FRES);
	/* clear interrupt status register */
	_SetISTR(0);
	/* Disable the Pull-Up*/
	USB_Cable_Config(DISABLE);
	/* switch-off device */
	_SetCNTR(CNTR_FRES + CNTR_PDWN);
	/* sw variables reset */
	/* ... */
	return USB_SUCCESS;
}

void Suspend(void){
	uint16_t wCNTR;
	/* macrocell enters suspend mode */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);
	/* force low-power mode in the macrocell */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);
	Enter_LowPowerMode();
}

void Resume_Init(void){
	uint16_t wCNTR;
	/* CNTR_LPMODE = 0 */
	wCNTR = _GetCNTR();
	wCNTR &= (~CNTR_LPMODE);
	_SetCNTR(wCNTR);
	Leave_LowPowerMode();
	/* reset FSUSP bit */
	_SetCNTR(IMR_MSK);
}

void Resume(RESUME_STATE eResumeSetVal){
	uint16_t wCNTR;

	if (eResumeSetVal != RESUME_ESOF)
	ResumeS.eState = eResumeSetVal;

	switch (ResumeS.eState){
	case RESUME_EXTERNAL:
		Resume_Init();
		ResumeS.eState = RESUME_OFF;
		break;
	case RESUME_INTERNAL:
		Resume_Init();
		ResumeS.eState = RESUME_START;
		break;
	case RESUME_LATER:
		ResumeS.bESOFcnt = 2;
		ResumeS.eState = RESUME_WAIT;
		break;
	case RESUME_WAIT:
		ResumeS.bESOFcnt--;
		if (ResumeS.bESOFcnt == 0)
			ResumeS.eState = RESUME_START;
		break;
	case RESUME_START:
		wCNTR = _GetCNTR();
		wCNTR |= CNTR_RESUME;
		_SetCNTR(wCNTR);
		ResumeS.eState = RESUME_ON;
		ResumeS.bESOFcnt = 10;
		break;
	case RESUME_ON:
		ResumeS.bESOFcnt--;
		if (ResumeS.bESOFcnt == 0){
			wCNTR = _GetCNTR();
			wCNTR &= (~CNTR_RESUME);
			_SetCNTR(wCNTR);
			ResumeS.eState = RESUME_OFF;
		}
		break;
	case RESUME_OFF:
	case RESUME_ESOF:
	default:
		ResumeS.eState = RESUME_OFF;
		break;
	}
}


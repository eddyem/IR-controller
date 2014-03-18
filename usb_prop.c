/*
 * usb_prop.c - USB init & properties
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

uint8_t Request = 0;

// USART line mode
LINE_CODING linecoding = {
	115200, // baud rate
	0x00,   // stop bits-1
	0x00,   // parity - none
	0x08    // no. of bits == 8
};

DEVICE Device_Table = {
	EP_NUM,  // total endpoints
	1        // total configurations
};

// pointers to main functions
DEVICE_PROP Device_Property = {
	SomeDev_init,
	SomeDev_Reset,
	SomeDev_Status_In,
	SomeDev_Status_Out,
	SomeDev_Data_Setup,
	SomeDev_NoData_Setup,
	SomeDev_Get_Interface_Setting,
	SomeDev_GetDeviceDescriptor,
	SomeDev_GetConfigDescriptor,
	SomeDev_GetStringDescriptor,
	0, // not used in this library version
	SomeDev_DATA_SIZE // max packet size
};


// interface functions
USER_STANDARD_REQUESTS User_Standard_Requests = {
	SomeDev_GetConfiguration,
	SomeDev_SetConfiguration,
	SomeDev_GetInterface,
	SomeDev_SetInterface,
	SomeDev_GetStatus,
	SomeDev_ClearFeature,
	SomeDev_SetEndPointFeature,
	SomeDev_SetDeviceFeature,
	SomeDev_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor = {
	(uint8_t*)SomeDev_DeviceDescriptor,
	SomeDev_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor = {
	(uint8_t*)SomeDev_ConfigDescriptor,
	SomeDev_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[SomeDev_SIZ_STRINGS] = {
	{(uint8_t*)SomeDev_StringLangID, SomeDev_SIZ_STRING_LANGID},
	{(uint8_t*)SomeDev_StringVendor, SomeDev_SIZ_STRING_VENDOR},
	{(uint8_t*)SomeDev_StringProduct, SomeDev_SIZ_STRING_PRODUCT},
	{(uint8_t*)SomeDev_StringSerial, SomeDev_SIZ_STRING_SERIAL}
};

// Device init
void SomeDev_init(){
	//Get_SerialNum();
	pInformation->Current_Configuration = 0;
	// power on
	PowerOn();
	// main initialisations taked from USB_SIL_Init();
	_SetISTR(0);
	wInterrupt_Mask = IMR_MSK;
	_SetCNTR(wInterrupt_Mask);
	// end of USB_SIL_Init()
	bDeviceState = UNCONNECTED;
}

// Reset device
void SomeDev_Reset(){
	pInformation->Current_Configuration = 0;
	pInformation->Current_Feature = SomeDev_ConfigDescriptor[7];
	// Set Virtual_Com_Port DEVICE with the default Interface
	pInformation->Current_Interface = 0;
	// endpoints initialisation
	SetBTABLE(BTABLE_ADDRESS);
	// EP0 - control
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_NAK);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxValid(ENDP0);
	// EP1 - data out
	SetEPType(ENDP1, EP_BULK);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPTxStatus(ENDP1, EP_TX_NAK);
	SetEPRxStatus(ENDP1, EP_RX_DIS);
	// EP2 - interrupts to PC
	SetEPType(ENDP2, EP_INTERRUPT);
	SetEPTxAddr(ENDP2, ENDP2_TXADDR);
	SetEPRxStatus(ENDP2, EP_RX_DIS);
	SetEPTxStatus(ENDP2, EP_TX_NAK);
	// EP3 - data in
	SetEPType(ENDP3, EP_BULK);
	SetEPRxAddr(ENDP3, ENDP3_RXADDR);
	SetEPRxCount(ENDP3, SomeDev_DATA_SIZE);
	SetEPRxStatus(ENDP3, EP_RX_VALID);
	SetEPTxStatus(ENDP3, EP_TX_DIS);
	// device address
	SetDeviceAddress(0);
	bDeviceState = ATTACHED;
}

// change configuration
void SomeDev_SetConfiguration(void){
	if (pInformation->Current_Configuration != 0){
		// device configured
		bDeviceState = CONFIGURED;
		// clear data toggle
		// ClearDTOG_TX(ENDP1);
		// ClearDTOG_RX(ENDP2);
	}
}

// device addressed
void SomeDev_SetDeviceAddress (void){
	bDeviceState = ADDRESSED;
}

void SomeDev_Status_In(void){
	if (Request == SET_LINE_CODING)
		Request = 0;
}

void SomeDev_Status_Out(void){
	return;
}

// setup data interface (only set/get line coding)
RESULT SomeDev_Data_Setup(uint8_t RequestNo){
	uint8_t *(*CopyRoutine)(uint16_t);
	CopyRoutine = NULL;
	if(RequestNo == GET_LINE_CODING){
		if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
			CopyRoutine = SomeDev_GetLineCoding;
	}
	else if (RequestNo == SET_LINE_CODING){
		if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
			CopyRoutine = SomeDev_SetLineCoding;
		Request = SET_LINE_CODING;
	}
	if (CopyRoutine == NULL){
		return USB_UNSUPPORT;
	}
	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);
	return USB_SUCCESS;
}


// setup nodata
RESULT SomeDev_NoData_Setup(uint8_t RequestNo){
	if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)){
		if (RequestNo == SET_COMM_FEATURE)
			return USB_SUCCESS;
		else if (RequestNo == SET_CONTROL_LINE_STATE)
			return USB_SUCCESS;
	}
	return USB_UNSUPPORT;
}

RESULT SomeDev_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting){
	if (AlternateSetting > 0)
		return USB_UNSUPPORT; // in this application we don't have AlternateSetting
	else if (Interface > 0)
		return USB_UNSUPPORT; // in this application we have only 1 interface
	return USB_SUCCESS;
}

uint8_t *SomeDev_GetDeviceDescriptor(uint16_t Length){
	return Standard_GetDescriptorData(Length, &Device_Descriptor );
}

uint8_t *SomeDev_GetConfigDescriptor(uint16_t Length){
	return Standard_GetDescriptorData(Length, &Config_Descriptor );
}

uint8_t *SomeDev_GetStringDescriptor(uint16_t Length){
	uint8_t wValue0 = pInformation->USBwValue0;
	if (wValue0 > SomeDev_SIZ_STRINGS)
		return NULL;
	else
		return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
}

uint8_t *SomeDev_GetLineCoding(uint16_t Length){
	if (Length == 0){
		pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
		return NULL;
	}
	return(uint8_t *)&linecoding;
}

uint8_t *SomeDev_SetLineCoding(uint16_t Length){
	if (Length == 0){
		pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
		return NULL;
	}
	// show current speed (we may reconfigure USART when this flag is set)
	FLAGS |= FLAG_PRINTSPD;
	return(uint8_t *)&linecoding;
}

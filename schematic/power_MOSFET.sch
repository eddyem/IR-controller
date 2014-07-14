EESchema Schematic File Version 2  date Пт 11 июл 2014 12:31:03
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:ft2232
LIBS:my_elements
LIBS:STM32f103
LIBS:vreg
LIBS:open-project
LIBS:ALL-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 10 12
Title "Power load module"
Date "11 jul 2014"
Rev ""
Comp "SAO RAS"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 3150 1250 0    61   Output ~ 0
GND_OUT
Text HLabel 3200 2800 0    61   Input ~ 0
GND
Text HLabel 3200 2550 0    61   Input ~ 0
ON
$Comp
L GND #PWRp0137
U 1 1 53AF4F61
P 3300 2900
AR Path="/53BBC16A/53AF4F61" Ref="#PWRp0137"  Part="1" 
AR Path="/53C73959/53AF4F61" Ref="#PWRp0134"  Part="1" 
AR Path="/53C7395A/53AF4F61" Ref="#PWRp0140"  Part="1" 
AR Path="/53C73958/53AF4F61" Ref="#PWRp086"  Part="1" 
AR Path="/53A8FBFD/53AF4F61" Ref="#PWRp0127"  Part="1" 
AR Path="/53AA0E2D/53AF4F61" Ref="#PWRp0129"  Part="1" 
AR Path="/53AA0E2C/53AF4F61" Ref="#PWRp0135"  Part="1" 
F 0 "#PWRp0129" H 3300 2900 30  0001 C CNN
F 1 "GND" H 3300 2830 30  0001 C CNN
F 2 "" H 3300 2900 60  0000 C CNN
F 3 "" H 3300 2900 60  0000 C CNN
	1    3300 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2800 3300 2800
Wire Wire Line
	3300 2800 3300 2900
$Comp
L NTE2984 Qp1.1
U 1 1 53AF4F62
P 5050 2550
AR Path="/53BBC16A/53AF4F62" Ref="Qp1.1"  Part="1" 
AR Path="/53C73959/53AF4F62" Ref="Qp1.2"  Part="1" 
AR Path="/53C7395A/53AF4F62" Ref="Qp1.3"  Part="1" 
AR Path="/53C73958/53AF4F62" Ref="Qp1.1"  Part="1" 
AR Path="/53A8FBFD/53AF4F62" Ref="Qp1.2"  Part="1" 
AR Path="/53AA0E2D/53AF4F62" Ref="Qp1.1"  Part="1" 
AR Path="/53AA0E2C/53AF4F62" Ref="Qp1.2"  Part="1" 
F 0 "Qp1.2" H 4970 2790 60  0000 R CNN
F 1 "NTE2984" H 5090 2310 60  0000 R CNN
F 2 "TO220-3-vert" H 5050 2550 60  0001 C CNN
F 3 "~" H 5050 2550 60  0000 C CNN
	1    5050 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2550 4850 2550
Wire Wire Line
	3150 1250 5150 1250
Wire Wire Line
	5150 1250 5150 2350
$Comp
L GND #PWRp0138
U 1 1 53AF4F63
P 5150 2850
AR Path="/53BBC16A/53AF4F63" Ref="#PWRp0138"  Part="1" 
AR Path="/53C73959/53AF4F63" Ref="#PWRp0135"  Part="1" 
AR Path="/53C7395A/53AF4F63" Ref="#PWRp0141"  Part="1" 
AR Path="/53C73958/53AF4F63" Ref="#PWRp087"  Part="1" 
AR Path="/53A8FBFD/53AF4F63" Ref="#PWRp0128"  Part="1" 
AR Path="/53AA0E2D/53AF4F63" Ref="#PWRp0130"  Part="1" 
AR Path="/53AA0E2C/53AF4F63" Ref="#PWRp0136"  Part="1" 
F 0 "#PWRp0130" H 5150 2850 30  0001 C CNN
F 1 "GND" H 5150 2780 30  0001 C CNN
F 2 "" H 5150 2850 60  0000 C CNN
F 3 "" H 5150 2850 60  0000 C CNN
	1    5150 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 2850 5150 2750
$EndSCHEMATC
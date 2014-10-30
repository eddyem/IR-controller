EESchema Schematic File Version 2  date Вт 07 окт 2014 17:18:47
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
Sheet 5 12
Title "RS-232 level converter"
Date "7 oct 2014"
Rev ""
Comp "SAO RAS"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 1550 1900 0    60   Input ~ 0
+5V
Text HLabel 1550 2100 0    60   Input ~ 0
GND
$Comp
L GND #PWR079
U 1 1 53974EEB
P 1600 2200
F 0 "#PWR079" H 1600 2200 30  0001 C CNN
F 1 "GND" H 1600 2130 30  0001 C CNN
F 2 "" H 1600 2200 60  0000 C CNN
F 3 "" H 1600 2200 60  0000 C CNN
	1    1600 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1600 2200 1600 2100
Wire Wire Line
	1600 2100 1550 2100
Wire Wire Line
	1550 1900 1600 1900
Wire Wire Line
	1600 1900 1600 1850
$Comp
L GND #PWR080
U 1 1 53975227
P 5250 3400
F 0 "#PWR080" H 5250 3400 30  0001 C CNN
F 1 "GND" H 5250 3330 30  0001 C CNN
F 2 "" H 5250 3400 60  0000 C CNN
F 3 "" H 5250 3400 60  0000 C CNN
	1    5250 3400
	1    0    0    -1  
$EndComp
$Comp
L DB9 XS1
U 1 1 53975A15
P 5700 2900
F 0 "XS1" H 5700 3450 70  0000 C CNN
F 1 "DB9" H 5700 2350 70  0000 C CNN
F 2 "DB9FC" H 5700 2900 60  0001 C CNN
F 3 "" H 5700 2900 60  0000 C CNN
	1    5700 2900
	1    0    0    1   
$EndComp
NoConn ~ 5250 2600
Wire Wire Line
	3950 2700 5250 2700
Wire Wire Line
	3950 2900 5250 2900
Wire Wire Line
	3950 3650 3950 3350
Wire Wire Line
	1600 3650 3950 3650
Wire Wire Line
	3950 3250 4000 3250
Wire Wire Line
	4000 3250 4000 3800
Wire Wire Line
	4000 3800 1600 3800
Wire Wire Line
	3950 3150 4050 3150
Wire Wire Line
	4050 3150 4050 3950
Wire Wire Line
	4050 3950 1600 3950
Wire Wire Line
	3950 3050 4100 3050
Wire Wire Line
	4100 3050 4100 4100
Wire Wire Line
	4100 4100 1600 4100
Text HLabel 1600 3650 0    60   Input ~ 0
UART_RX
Text HLabel 1600 3800 0    60   Input ~ 0
UART_RX1
Text HLabel 1600 3950 0    60   Input ~ 0
UART_TX
Text HLabel 1600 4100 0    60   Input ~ 0
UART_TX1
$Comp
L +5V #PWR081
U 1 1 53A48DE6
P 1600 1850
F 0 "#PWR081" H 1600 1940 20  0001 C CNN
F 1 "+5V" H 1600 1940 30  0000 C CNN
F 2 "" H 1600 1850 60  0000 C CNN
F 3 "" H 1600 1850 60  0000 C CNN
	1    1600 1850
	1    0    0    -1  
$EndComp
$Comp
L ADM233L DD1
U 1 1 53A48DFB
P 3450 2650
F 0 "DD1" H 3250 3100 60  0000 C CNN
F 1 "ADM233L" H 3450 1750 60  0000 C CNN
F 2 "DIP-20__300" H 3450 2650 60  0001 C CNN
F 3 "~" H 3450 2650 60  0000 C CNN
	1    3450 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 2400 3950 2500
NoConn ~ 3950 2300
NoConn ~ 2950 2750
NoConn ~ 2950 2850
Wire Wire Line
	2950 3000 2950 3100
Wire Wire Line
	2950 3250 2950 3350
Wire Wire Line
	2950 2500 2950 2600
Wire Wire Line
	2950 2550 2850 2550
Wire Wire Line
	2850 2550 2850 2600
Connection ~ 2950 2550
$Comp
L GND #PWR082
U 1 1 53A4A7BE
P 2850 2600
F 0 "#PWR082" H 2850 2600 30  0001 C CNN
F 1 "GND" H 2850 2530 30  0001 C CNN
F 2 "" H 2850 2600 60  0000 C CNN
F 3 "" H 2850 2600 60  0000 C CNN
	1    2850 2600
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR083
U 1 1 53A4A7C4
P 2950 2250
F 0 "#PWR083" H 2950 2340 20  0001 C CNN
F 1 "+5V" H 2950 2340 30  0000 C CNN
F 2 "" H 2950 2250 60  0000 C CNN
F 3 "" H 2950 2250 60  0000 C CNN
	1    2950 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 2250 2950 2350
Text Notes 6700 2300 0    100  ~ 0
DB9 pinout:\n1 - Data carrier detect\n2 - Receive data\n3 - Transmit data\n4 - Data terminal ready\n5 - Signal ground\n6 - Data set ready\n7 - Request to send\n8 - Clear to send\n9 - Ring indicator
Text Notes 5300 2250 0    60   ~ 0
Main UART (socket)
NoConn ~ 5250 3200
NoConn ~ 5250 2800
NoConn ~ 5250 3000
NoConn ~ 5250 3100
Wire Wire Line
	5250 3400 5250 3300
NoConn ~ 5250 2500
$Comp
L GND #PWR084
U 1 1 53AB443D
P 5250 4800
F 0 "#PWR084" H 5250 4800 30  0001 C CNN
F 1 "GND" H 5250 4730 30  0001 C CNN
F 2 "" H 5250 4800 60  0000 C CNN
F 3 "" H 5250 4800 60  0000 C CNN
	1    5250 4800
	1    0    0    -1  
$EndComp
$Comp
L DB9 XP6
U 1 1 53AB4443
P 5700 4300
F 0 "XP6" H 5700 4850 70  0000 C CNN
F 1 "DB9" H 5700 3750 70  0000 C CNN
F 2 "DB9FC" H 5700 4300 60  0001 C CNN
F 3 "" H 5700 4300 60  0000 C CNN
	1    5700 4300
	1    0    0    1   
$EndComp
NoConn ~ 5250 4000
Text Notes 5300 3650 0    60   ~ 0
Reserved UART (plug)
NoConn ~ 5250 4600
NoConn ~ 5250 4200
NoConn ~ 5250 4400
NoConn ~ 5250 4500
Wire Wire Line
	5250 4800 5250 4700
NoConn ~ 5250 3900
Wire Wire Line
	3950 2800 4550 2800
Wire Wire Line
	4550 2800 4550 4300
Wire Wire Line
	4550 4300 5250 4300
Wire Wire Line
	5250 4100 4750 4100
Wire Wire Line
	4750 4100 4750 2600
Wire Wire Line
	4750 2600 3950 2600
$EndSCHEMATC

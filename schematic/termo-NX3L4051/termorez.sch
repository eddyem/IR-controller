EESchema Schematic File Version 2  date Сб 27 дек 2014 18:53:43
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
LIBS:ad
LIBS:analog-devices
LIBS:analog-devices2
LIBS:my_elements
LIBS:termorez-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "noname.sch"
Date "27 dec 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L AD7794/7795 U4
U 1 1 524D62C1
P 6250 3650
F 0 "U4" H 6250 4550 60  0000 C CNN
F 1 "AD7794/7795" H 6250 2750 60  0000 C CNN
F 2 "" H 6250 3750 60  0000 C CNN
F 3 "" H 6250 3750 60  0000 C CNN
	1    6250 3650
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 53562118
P 5150 5000
F 0 "R4" V 5230 5000 40  0000 C CNN
F 1 "1k 0.1% (RT0805BRB071KL)" V 5050 5000 40  0000 C CNN
F 2 "SM0805" V 5080 5000 30  0001 C CNN
F 3 "~" H 5150 5000 30  0000 C CNN
	1    5150 5000
	0    -1   -1   0   
$EndComp
Entry Wire Line
	4200 1800 4300 1900
Entry Wire Line
	4200 1900 4300 2000
Entry Wire Line
	4200 2000 4300 2100
Text Label 4200 1800 2    60   ~ 0
A0
Text Label 4200 1900 2    60   ~ 0
A1
Text Label 4200 2000 2    60   ~ 0
A2
Entry Wire Line
	4200 2200 4300 2300
Text Label 4200 2200 2    60   ~ 0
EN
$Comp
L AGND #PWR01
U 1 1 53576A0C
P 3500 2800
F 0 "#PWR01" H 3500 2800 40  0001 C CNN
F 1 "AGND" H 3500 2730 50  0000 C CNN
F 2 "" H 3500 2800 60  0000 C CNN
F 3 "" H 3500 2800 60  0000 C CNN
	1    3500 2800
	1    0    0    -1  
$EndComp
$Comp
L CP1 C12
U 1 1 5357750C
P 9400 6450
F 0 "C12" H 9400 6550 40  0000 L CNN
F 1 "47u" H 9406 6365 40  0000 L CNN
F 2 "" H 9438 6300 30  0001 C CNN
F 3 "~" H 9400 6450 60  0000 C CNN
	1    9400 6450
	1    0    0    -1  
$EndComp
$Comp
L INDUCTOR_SMALL L1
U 1 1 53577523
P 8700 5750
F 0 "L1" H 8700 5850 50  0000 C CNN
F 1 "BMB2A0300AN1" H 8700 5700 50  0000 C CNN
F 2 "~" H 8700 5750 60  0000 C CNN
F 3 "~" H 8700 5750 60  0000 C CNN
	1    8700 5750
	0    -1   -1   0   
$EndComp
$Comp
L DGND #PWR02
U 1 1 535776AA
P 9400 6750
F 0 "#PWR02" H 9400 6750 40  0001 C CNN
F 1 "DGND" H 9400 6680 40  0000 C CNN
F 2 "" H 9400 6750 60  0000 C CNN
F 3 "" H 9400 6750 60  0000 C CNN
	1    9400 6750
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR03
U 1 1 535776B9
P 9100 5500
F 0 "#PWR03" H 9100 5500 40  0001 C CNN
F 1 "AGND" H 9100 5430 50  0000 C CNN
F 2 "" H 9100 5500 60  0000 C CNN
F 3 "" H 9100 5500 60  0000 C CNN
	1    9100 5500
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR04
U 1 1 5357776D
P 9300 7050
F 0 "#PWR04" H 9300 7050 40  0001 C CNN
F 1 "AGND" H 9300 6980 50  0000 C CNN
F 2 "" H 9300 7050 60  0000 C CNN
F 3 "" H 9300 7050 60  0000 C CNN
	1    9300 7050
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 53576FB6
P 8700 6450
F 0 "C9" H 8700 6550 40  0000 L CNN
F 1 "0.1u" H 8706 6365 40  0000 L CNN
F 2 "~" H 8738 6300 30  0000 C CNN
F 3 "~" H 8700 6450 60  0000 C CNN
	1    8700 6450
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 53576FC3
P 7750 6450
F 0 "C8" H 7750 6550 40  0000 L CNN
F 1 "0.33u" H 7756 6365 40  0000 L CNN
F 2 "~" H 7788 6300 30  0000 C CNN
F 3 "~" H 7750 6450 60  0000 C CNN
	1    7750 6450
	1    0    0    -1  
$EndComp
Text GLabel 7350 5800 0    60   Input ~ 0
DVDD
Wire Wire Line
	9100 5400 9100 5500
Wire Wire Line
	9400 6750 9400 6650
Wire Wire Line
	8700 5300 8700 5500
Wire Wire Line
	9400 6250 9400 6150
Wire Wire Line
	8650 6250 9400 6250
Connection ~ 8700 6250
Wire Wire Line
	7350 6250 7850 6250
$Comp
L DGND #PWR05
U 1 1 53577FCA
P 8200 7050
F 0 "#PWR05" H 8200 7050 40  0001 C CNN
F 1 "DGND" H 8200 6980 40  0000 C CNN
F 2 "" H 8200 7050 60  0000 C CNN
F 3 "" H 8200 7050 60  0000 C CNN
	1    8200 7050
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR06
U 1 1 53577FD0
P 7750 6750
F 0 "#PWR06" H 7750 6750 40  0001 C CNN
F 1 "DGND" H 7750 6680 40  0000 C CNN
F 2 "" H 7750 6750 60  0000 C CNN
F 3 "" H 7750 6750 60  0000 C CNN
	1    7750 6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7750 6750 7750 6650
Wire Wire Line
	7750 6150 7750 6250
$Comp
L DGND #PWR07
U 1 1 53578546
P 8250 6650
F 0 "#PWR07" H 8250 6650 40  0001 C CNN
F 1 "DGND" H 8250 6580 40  0000 C CNN
F 2 "" H 8250 6650 60  0000 C CNN
F 3 "" H 8250 6650 60  0000 C CNN
	1    8250 6650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8250 6650 8250 6550
$Comp
L DGND #PWR08
U 1 1 5357863B
P 6950 3500
F 0 "#PWR08" H 6950 3500 40  0001 C CNN
F 1 "DGND" H 6950 3430 40  0000 C CNN
F 2 "" H 6950 3500 60  0000 C CNN
F 3 "" H 6950 3500 60  0000 C CNN
	1    6950 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3500 6950 3400
$Comp
L LM317AEMP U5
U 1 1 535787B7
P 8250 6300
F 0 "U5" H 8050 6500 40  0000 C CNN
F 1 "LM1117-33" H 8250 6500 40  0000 L CNN
F 2 "SOT-223" H 8250 6400 30  0000 C CIN
F 3 "" H 8250 6300 60  0000 C CNN
	1    8250 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 4300 5550 4300
Wire Wire Line
	5400 4200 5550 4200
$Comp
L AGND #PWR09
U 1 1 53578BFD
P 5400 6200
F 0 "#PWR09" H 5400 6200 40  0001 C CNN
F 1 "AGND" H 5400 6130 50  0000 C CNN
F 2 "" H 5400 6200 60  0000 C CNN
F 3 "" H 5400 6200 60  0000 C CNN
	1    5400 6200
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR010
U 1 1 53578C83
P 5550 4500
F 0 "#PWR010" H 5550 4500 40  0001 C CNN
F 1 "AGND" H 5550 4430 50  0000 C CNN
F 2 "" H 5550 4500 60  0000 C CNN
F 3 "" H 5550 4500 60  0000 C CNN
	1    5550 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 4500 5550 4400
Wire Wire Line
	5200 3750 5550 3750
Wire Wire Line
	5200 3050 5200 3750
Wire Wire Line
	5200 3050 5550 3050
Wire Wire Line
	5000 3850 5550 3850
Wire Wire Line
	3850 2950 5550 2950
Connection ~ 5000 2950
Wire Wire Line
	5550 3150 5450 3150
Wire Wire Line
	5450 3150 5450 3650
Wire Wire Line
	5450 3250 5550 3250
Wire Wire Line
	5350 3350 5550 3350
Connection ~ 5450 3250
Wire Wire Line
	5450 3450 5550 3450
Connection ~ 5450 3350
Wire Wire Line
	5450 3550 5550 3550
Connection ~ 5450 3450
Wire Wire Line
	5450 3650 5550 3650
Connection ~ 5450 3550
$Comp
L AGND #PWR011
U 1 1 53579301
P 5350 3400
F 0 "#PWR011" H 5350 3400 40  0001 C CNN
F 1 "AGND" H 5350 3330 50  0000 C CNN
F 2 "" H 5350 3400 60  0000 C CNN
F 3 "" H 5350 3400 60  0000 C CNN
	1    5350 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 3400 5350 3350
$Comp
L R R1
U 1 1 53579396
P 4550 3800
F 0 "R1" V 4630 3800 40  0000 C CNN
F 1 "10k" V 4557 3801 40  0000 C CNN
F 2 "~" V 4480 3800 30  0000 C CNN
F 3 "~" H 4550 3800 30  0000 C CNN
	1    4550 3800
	0    1    1    0   
$EndComp
$Comp
L R R2
U 1 1 535793A8
P 4550 3950
F 0 "R2" V 4630 3950 40  0000 C CNN
F 1 "10k" V 4557 3951 40  0000 C CNN
F 2 "~" V 4480 3950 30  0000 C CNN
F 3 "~" H 4550 3950 30  0000 C CNN
	1    4550 3950
	0    1    1    0   
$EndComp
Wire Wire Line
	4800 3800 4900 3800
Wire Wire Line
	4900 3800 4900 3950
Wire Wire Line
	4900 3950 5550 3950
Wire Wire Line
	4800 3950 4800 4050
Wire Wire Line
	4800 4050 5550 4050
Wire Wire Line
	4300 3800 4300 4050
$Comp
L AGND #PWR012
U 1 1 535795F6
P 4300 4050
F 0 "#PWR012" H 4300 4050 40  0001 C CNN
F 1 "AGND" H 4300 3980 50  0000 C CNN
F 2 "" H 4300 4050 60  0000 C CNN
F 3 "" H 4300 4050 60  0000 C CNN
	1    4300 4050
	1    0    0    -1  
$EndComp
Connection ~ 4300 3950
$Comp
L AGND #PWR013
U 1 1 5357976D
P 6950 3100
F 0 "#PWR013" H 6950 3100 40  0001 C CNN
F 1 "AGND" H 6950 3030 50  0000 C CNN
F 2 "" H 6950 3100 60  0000 C CNN
F 3 "" H 6950 3100 60  0000 C CNN
	1    6950 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3100 6950 3000
Wire Wire Line
	6950 2800 6950 2900
Wire Wire Line
	6950 3850 6950 3950
$Comp
L CONN_8 P1
U 1 1 53579853
P 2050 2050
F 0 "P1" V 2050 2050 60  0000 C CNN
F 1 "контакт 1" H 2000 1550 60  0000 C CNN
F 2 "" H 2050 2150 60  0000 C CNN
F 3 "" H 2050 2150 60  0000 C CNN
	1    2050 2050
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2650 1700 2400 1700
Wire Wire Line
	2400 1800 2650 1800
Wire Wire Line
	2650 1900 2400 1900
Wire Wire Line
	2400 2000 2650 2000
Wire Wire Line
	2650 2100 2400 2100
Wire Wire Line
	2400 2200 2650 2200
Wire Wire Line
	2650 2300 2400 2300
Wire Wire Line
	2400 2400 2650 2400
Text Notes 2300 1500 2    60   ~ 0
К терморезисторам, контакт 1
Wire Bus Line
	4300 1900 4300 3500
Wire Bus Line
	7850 1900 7850 4650
$Comp
L DGND #PWR014
U 1 1 5357A718
P 7200 4200
F 0 "#PWR014" H 7200 4200 40  0001 C CNN
F 1 "DGND" H 7200 4130 40  0000 C CNN
F 2 "" H 7200 4200 60  0000 C CNN
F 3 "" H 7200 4200 60  0000 C CNN
	1    7200 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 4200 7200 4100
Wire Wire Line
	7200 4100 6950 4100
$Comp
L CONN_2 P6
U 1 1 5357ABB4
P 7000 6350
F 0 "P6" V 6950 6350 40  0000 C CNN
F 1 "5V" V 7050 6350 40  0000 C CNN
F 2 "" H 7000 6350 60  0000 C CNN
F 3 "" H 7000 6350 60  0000 C CNN
	1    7000 6350
	-1   0    0    1   
$EndComp
Text Notes 6850 6100 0    60   ~ 0
DVDD ext
Connection ~ 9400 6250
Wire Wire Line
	8700 6000 8700 6250
Text GLabel 7350 6650 0    60   Input ~ 0
GND
$Comp
L VDD #PWR015
U 1 1 5357B336
P 9400 6150
F 0 "#PWR015" H 9400 6250 30  0001 C CNN
F 1 "VDD" H 9400 6260 30  0000 C CNN
F 2 "" H 9400 6150 60  0000 C CNN
F 3 "" H 9400 6150 60  0000 C CNN
	1    9400 6150
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR016
U 1 1 5357B33C
P 6950 3850
F 0 "#PWR016" H 6950 3950 30  0001 C CNN
F 1 "VDD" H 6950 3960 30  0000 C CNN
F 2 "" H 6950 3850 60  0000 C CNN
F 3 "" H 6950 3850 60  0000 C CNN
	1    6950 3850
	1    0    0    -1  
$EndComp
Entry Wire Line
	6950 4200 7050 4300
Entry Wire Line
	6950 4300 7050 4400
Entry Wire Line
	6950 4400 7050 4500
Text Label 6950 4200 2    60   ~ 0
SC
Text Label 6950 4300 0    60   ~ 0
SDO
Text Label 6950 4400 0    60   ~ 0
SDI
Wire Bus Line
	7050 4300 7050 4650
Wire Bus Line
	7050 4650 7850 4650
$Comp
L CONN_8 P4
U 1 1 5357BA72
P 8450 3200
F 0 "P4" V 8400 3200 60  0000 C CNN
F 1 "MCU" V 8500 3200 60  0000 C CNN
F 2 "" H 8450 3200 60  0000 C CNN
F 3 "" H 8450 3200 60  0000 C CNN
	1    8450 3200
	1    0    0    -1  
$EndComp
Entry Wire Line
	7850 2950 7950 2850
Entry Wire Line
	7850 3050 7950 2950
Entry Wire Line
	7850 3150 7950 3050
Entry Wire Line
	7850 3250 7950 3150
Entry Wire Line
	7850 3450 7950 3350
Entry Wire Line
	7850 3550 7950 3450
Entry Wire Line
	7850 3650 7950 3550
Wire Wire Line
	7650 2850 8100 2850
Wire Wire Line
	7950 2950 8100 2950
Wire Wire Line
	8100 3050 7950 3050
Wire Wire Line
	8100 3150 7950 3150
Wire Wire Line
	7950 3350 8100 3350
Wire Wire Line
	8100 3450 7950 3450
Wire Wire Line
	7950 3550 8100 3550
Text Label 7950 2850 0    60   ~ 0
EN
Text Label 7950 2950 0    60   ~ 0
A0
Text Label 7950 3050 0    60   ~ 0
A1
Text Label 7950 3150 0    60   ~ 0
A2
Text Label 7950 3350 0    60   ~ 0
SC
Text Label 7950 3450 0    60   ~ 0
SDO
Text Label 7950 3550 0    60   ~ 0
SDI
Text Notes 8650 2950 0    60   ~ 0
Цифровой интерфейс:\nEN - включение коммутатора\nA0..A3 - номер терморезистора\nSC - тактирование SPI\nSDO - MOSI SPI\nDSI - MISO SPI
$Comp
L PWR_FLAG #FLG017
U 1 1 5357C0C3
P 8700 5400
F 0 "#FLG017" H 8700 5495 30  0001 C CNN
F 1 "PWR_FLAG" H 8700 5580 30  0000 C CNN
F 2 "" H 8700 5400 60  0000 C CNN
F 3 "" H 8700 5400 60  0000 C CNN
	1    8700 5400
	0    -1   -1   0   
$EndComp
Connection ~ 8700 5400
$Comp
L PWR_FLAG #FLG018
U 1 1 5357C2B1
P 9400 6650
F 0 "#FLG018" H 9400 6745 30  0001 C CNN
F 1 "PWR_FLAG" H 9400 6830 30  0000 C CNN
F 2 "" H 9400 6650 60  0000 C CNN
F 3 "" H 9400 6650 60  0000 C CNN
	1    9400 6650
	0    -1   -1   0   
$EndComp
$Comp
L R R3
U 1 1 5357C8F8
P 7650 3100
F 0 "R3" V 7730 3100 40  0000 C CNN
F 1 "47k" V 7657 3101 40  0000 C CNN
F 2 "~" V 7580 3100 30  0000 C CNN
F 3 "~" H 7650 3100 30  0000 C CNN
	1    7650 3100
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR019
U 1 1 5357CA90
P 7650 3450
F 0 "#PWR019" H 7650 3450 40  0001 C CNN
F 1 "DGND" H 7650 3380 40  0000 C CNN
F 2 "" H 7650 3450 60  0000 C CNN
F 3 "" H 7650 3450 60  0000 C CNN
	1    7650 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 3450 7650 3350
Text Notes 2650 2900 2    60   ~ 0
К терморезисторам, контакт 2а
Text Notes 2600 4600 2    60   ~ 0
К терморезисторам, контакт 2б
$Comp
L CONN_8 P2
U 1 1 5475974D
P 2050 3650
F 0 "P2" V 2050 3650 60  0000 C CNN
F 1 "контакт 2a" H 2000 3150 60  0000 C CNN
F 2 "" H 2050 3750 60  0000 C CNN
F 3 "" H 2050 3750 60  0000 C CNN
	1    2050 3650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2650 3300 2400 3300
Wire Wire Line
	2400 3400 2650 3400
Wire Wire Line
	2650 3500 2400 3500
Wire Wire Line
	2400 3600 2650 3600
Wire Wire Line
	2650 3700 2400 3700
Wire Wire Line
	2400 3800 2650 3800
Wire Wire Line
	2650 3900 2400 3900
Wire Wire Line
	2400 4000 2650 4000
$Comp
L CONN_8 P3
U 1 1 5475975B
P 2050 5250
F 0 "P3" V 2050 5250 60  0000 C CNN
F 1 "контакт 2б" H 2000 4750 60  0000 C CNN
F 2 "" H 2050 5350 60  0000 C CNN
F 3 "" H 2050 5350 60  0000 C CNN
	1    2050 5250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2650 4900 2400 4900
Wire Wire Line
	2400 5000 2650 5000
Wire Wire Line
	2650 5100 2400 5100
Wire Wire Line
	2400 5200 2650 5200
Wire Wire Line
	2650 5300 2400 5300
Wire Wire Line
	2400 5400 2650 5400
Wire Wire Line
	2650 5500 2400 5500
Wire Wire Line
	2400 5600 2650 5600
$Comp
L NX3L4051-HX DA1
U 1 1 54759E36
P 3250 2050
F 0 "DA1" H 3500 2550 60  0000 C CNN
F 1 "NX3L4051-HX" H 3650 1550 60  0000 C CNN
F 2 "HXQFN-16" H 3650 2750 60  0001 C CNN
F 3 "" H 3250 2050 60  0000 C CNN
	1    3250 2050
	-1   0    0    -1  
$EndComp
$Comp
L NX3L4051-HX DA3
U 1 1 5475A2C7
P 3250 5250
F 0 "DA3" H 3500 5750 60  0000 C CNN
F 1 "NX3L4051-HX" H 3650 4750 60  0000 C CNN
F 2 "HXQFN-16" H 3650 5950 60  0001 C CNN
F 3 "" H 3250 5250 60  0000 C CNN
	1    3250 5250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3850 2400 3850 2950
Wire Wire Line
	5000 2950 5000 3850
Wire Wire Line
	4200 1800 3850 1800
Wire Wire Line
	4200 1900 3850 1900
Wire Wire Line
	3850 2000 4200 2000
Wire Wire Line
	3850 2200 4200 2200
Wire Wire Line
	3500 2800 3500 2750
Wire Wire Line
	3500 2750 3250 2750
Wire Wire Line
	3600 4400 3600 4350
Wire Wire Line
	3600 4350 3250 4350
Wire Wire Line
	3600 6000 3600 5950
Wire Wire Line
	3600 5950 3250 5950
$Comp
L NX3L4051-HX DA2
U 1 1 5475A2C1
P 3250 3650
F 0 "DA2" H 3500 4150 60  0000 C CNN
F 1 "NX3L4051-HX" H 3650 3150 60  0000 C CNN
F 2 "HXQFN-16" H 3650 4350 60  0001 C CNN
F 3 "" H 3250 3650 60  0000 C CNN
	1    3250 3650
	-1   0    0    -1  
$EndComp
$Comp
L AGND #PWR020
U 1 1 5475AA5F
P 3600 4400
F 0 "#PWR020" H 3600 4400 40  0001 C CNN
F 1 "AGND" H 3600 4330 50  0000 C CNN
F 2 "" H 3600 4400 60  0000 C CNN
F 3 "" H 3600 4400 60  0000 C CNN
	1    3600 4400
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR021
U 1 1 5475AA65
P 3600 6000
F 0 "#PWR021" H 3600 6000 40  0001 C CNN
F 1 "AGND" H 3600 5930 50  0000 C CNN
F 2 "" H 3600 6000 60  0000 C CNN
F 3 "" H 3600 6000 60  0000 C CNN
	1    3600 6000
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR022
U 1 1 5475AA6D
P 6950 2800
F 0 "#PWR022" H 6950 2760 30  0001 C CNN
F 1 "+3.3V" H 6950 2910 30  0000 C CNN
F 2 "" H 6950 2800 60  0000 C CNN
F 3 "" H 6950 2800 60  0000 C CNN
	1    6950 2800
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR023
U 1 1 5475AAB6
P 8700 5300
F 0 "#PWR023" H 8700 5260 30  0001 C CNN
F 1 "+3.3V" H 8700 5410 30  0000 C CNN
F 2 "" H 8700 5300 60  0000 C CNN
F 3 "" H 8700 5300 60  0000 C CNN
	1    8700 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 4000 4100 4000
Wire Wire Line
	4100 4000 4100 3200
Wire Wire Line
	4100 3200 5200 3200
Connection ~ 5200 3200
Wire Wire Line
	3850 5600 4500 5600
Wire Wire Line
	4500 5600 4500 4300
Entry Wire Line
	3900 3400 4000 3500
Entry Wire Line
	3900 3500 4000 3600
Entry Wire Line
	3900 3600 4000 3700
Text Label 3900 3400 2    60   ~ 0
A0
Text Label 3900 3500 2    60   ~ 0
A1
Text Label 3900 3600 2    60   ~ 0
A2
Entry Wire Line
	3900 3800 4000 3900
Text Label 3900 3800 2    60   ~ 0
EN
Entry Wire Line
	3900 5000 4000 5100
Entry Wire Line
	3900 5100 4000 5200
Entry Wire Line
	3900 5200 4000 5300
Text Label 3900 5000 2    60   ~ 0
A0
Text Label 3900 5100 2    60   ~ 0
A1
Text Label 3900 5200 2    60   ~ 0
A2
Entry Wire Line
	3900 5400 4000 5500
Text Label 3900 5400 2    60   ~ 0
EN
Wire Wire Line
	3850 3400 3900 3400
Wire Wire Line
	3900 3500 3850 3500
Wire Wire Line
	3850 3600 3900 3600
Wire Wire Line
	3900 3800 3850 3800
Wire Wire Line
	3850 5000 3900 5000
Wire Wire Line
	3900 5100 3850 5100
Wire Wire Line
	3900 5200 3850 5200
Wire Wire Line
	3900 5400 3850 5400
Wire Wire Line
	3250 1300 3250 1350
Wire Wire Line
	3150 2950 3250 2950
Wire Wire Line
	3150 4550 3250 4550
Wire Bus Line
	4300 3500 4000 3500
Wire Bus Line
	4000 3500 4000 5500
Wire Bus Line
	4300 1900 7850 1900
Text Label 7950 3250 0    60   ~ 0
GND
Wire Wire Line
	7950 3250 8100 3250
Text Label 7450 3350 2    60   ~ 0
GND
Wire Wire Line
	7650 3350 7450 3350
Connection ~ 7650 3350
Wire Wire Line
	7350 5800 7350 6250
Wire Wire Line
	7350 6650 7350 6450
Wire Wire Line
	7750 6650 7350 6650
Connection ~ 7750 6250
Connection ~ 7350 6250
Connection ~ 7350 6650
Connection ~ 7750 6650
$Comp
L C C10
U 1 1 5357821E
P 8900 5400
F 0 "C10" H 8900 5500 40  0000 L CNN
F 1 "1u" H 8906 5315 40  0000 L CNN
F 2 "~" H 8938 5250 30  0000 C CNN
F 3 "~" H 8900 5400 60  0000 C CNN
	1    8900 5400
	0    -1   -1   0   
$EndComp
$Comp
L PWR_FLAG #FLG027
U 1 1 547C0AD7
P 7750 6150
F 0 "#FLG027" H 7750 6245 30  0001 C CNN
F 1 "PWR_FLAG" H 7750 6330 30  0000 C CNN
F 2 "" H 7750 6150 60  0000 C CNN
F 3 "" H 7750 6150 60  0000 C CNN
	1    7750 6150
	1    0    0    -1  
$EndComp
Connection ~ 4900 4300
$Comp
L R R5
U 1 1 547C0DD9
P 5150 5250
F 0 "R5" V 5230 5250 40  0000 C CNN
F 1 "1k 0.1% (RT0805BRB071KL)" V 5050 5250 40  0000 C CNN
F 2 "SM0805" V 5080 5250 30  0001 C CNN
F 3 "~" H 5150 5250 30  0000 C CNN
	1    5150 5250
	0    -1   -1   0   
$EndComp
$Comp
L R R6
U 1 1 547C0DE4
P 5150 5500
F 0 "R6" V 5230 5500 40  0000 C CNN
F 1 "1k 0.1% (RT0805BRB071KL)" V 5050 5500 40  0000 C CNN
F 2 "SM0805" V 5080 5500 30  0001 C CNN
F 3 "~" H 5150 5500 30  0000 C CNN
	1    5150 5500
	0    -1   -1   0   
$EndComp
$Comp
L R R7
U 1 1 547C0DEA
P 5150 5750
F 0 "R7" V 5230 5750 40  0000 C CNN
F 1 "1k 0.1% (RT0805BRB071KL)" V 5050 5750 40  0000 C CNN
F 2 "SM0805" V 5080 5750 30  0001 C CNN
F 3 "~" H 5150 5750 30  0000 C CNN
	1    5150 5750
	0    -1   -1   0   
$EndComp
$Comp
L R R8
U 1 1 547C0DF0
P 5150 6000
F 0 "R8" V 5230 6000 40  0000 C CNN
F 1 "1k 0.1% (RT0805BRB071KL)" V 5050 6000 40  0000 C CNN
F 2 "SM0805" V 5080 6000 30  0001 C CNN
F 3 "~" H 5150 6000 30  0000 C CNN
	1    5150 6000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4900 5000 4900 4300
Wire Wire Line
	5400 5000 5400 5250
Wire Wire Line
	4900 5500 4900 5250
Wire Wire Line
	5400 5500 5400 5750
Wire Wire Line
	4900 5750 4900 6000
Wire Wire Line
	5400 6200 5400 6000
Wire Wire Line
	5400 6000 5600 6000
Wire Wire Line
	5600 6000 5600 4800
Wire Wire Line
	5600 4800 5400 4800
Wire Wire Line
	5400 4800 5400 4200
$Comp
L GND_CONNECTOR X1
U 1 1 549EDA1E
P 8750 7000
F 0 "X1" H 8750 7100 60  0000 C CNN
F 1 "GND_CONNECTOR" H 8750 6850 60  0000 C CNN
F 2 "GND_CONNECTOR" H 8650 7000 60  0001 C CNN
F 3 "" H 8750 7000 60  0000 C CNN
	1    8750 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 7000 9300 7000
Wire Wire Line
	9300 7000 9300 7050
Wire Wire Line
	8200 7050 8200 7000
Wire Wire Line
	8200 7000 8450 7000
Wire Wire Line
	8450 7000 8450 6650
Wire Wire Line
	8450 6650 8700 6650
$Comp
L PWR_FLAG #FLG028
U 1 1 549EDD64
P 9050 7000
F 0 "#FLG028" H 9050 7095 30  0001 C CNN
F 1 "PWR_FLAG" H 9050 7180 30  0000 C CNN
F 2 "" H 9050 7000 60  0000 C CNN
F 3 "" H 9050 7000 60  0000 C CNN
	1    9050 7000
	1    0    0    -1  
$EndComp
Connection ~ 9050 7000
$Comp
L VDD #PWR?
U 1 1 549EE075
P 3150 4550
F 0 "#PWR?" H 3150 4650 30  0001 C CNN
F 1 "VDD" H 3150 4660 30  0000 C CNN
F 2 "" H 3150 4550 60  0000 C CNN
F 3 "" H 3150 4550 60  0000 C CNN
	1    3150 4550
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR?
U 1 1 549EE07B
P 3150 2950
F 0 "#PWR?" H 3150 3050 30  0001 C CNN
F 1 "VDD" H 3150 3060 30  0000 C CNN
F 2 "" H 3150 2950 60  0000 C CNN
F 3 "" H 3150 2950 60  0000 C CNN
	1    3150 2950
	1    0    0    -1  
$EndComp
$Comp
L VDD #PWR?
U 1 1 549EE081
P 3250 1300
F 0 "#PWR?" H 3250 1400 30  0001 C CNN
F 1 "VDD" H 3250 1410 30  0000 C CNN
F 2 "" H 3250 1300 60  0000 C CNN
F 3 "" H 3250 1300 60  0000 C CNN
	1    3250 1300
	1    0    0    -1  
$EndComp
$EndSCHEMATC

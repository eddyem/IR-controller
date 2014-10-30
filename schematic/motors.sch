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
Sheet 2 12
Title "Stepper motor module"
Date "7 oct 2014"
Rev ""
Comp "SAO RAS"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3500 3550 3700 3550
Wire Wire Line
	5950 3650 5400 3650
Wire Wire Line
	5400 3650 5400 4350
Wire Wire Line
	5400 4350 4900 4350
Wire Wire Line
	5950 3550 5300 3550
Wire Wire Line
	5300 3550 5300 3650
Wire Wire Line
	5300 3650 4900 3650
Wire Wire Line
	3700 3150 3500 3150
Connection ~ 5150 3250
Wire Wire Line
	4900 3250 5150 3250
Connection ~ 5750 2750
Wire Wire Line
	5750 2500 5750 2750
Connection ~ 5750 3250
Wire Wire Line
	5700 3250 5750 3250
Connection ~ 5250 3250
Wire Wire Line
	5250 3000 5250 3300
Wire Wire Line
	5300 3250 5250 3250
Connection ~ 4700 4800
Wire Wire Line
	5050 4800 4400 4800
Connection ~ 5050 4450
Wire Wire Line
	4900 4450 5050 4450
Wire Wire Line
	5300 2750 5250 2750
Wire Wire Line
	5000 2450 5000 4050
Wire Wire Line
	5000 4050 4900 4050
Connection ~ 4100 4650
Connection ~ 3900 4650
Wire Wire Line
	2750 4050 3700 4050
Wire Wire Line
	5050 5300 5050 5200
Wire Wire Line
	4700 5300 5050 5300
Connection ~ 4850 5300
Wire Wire Line
	4400 5300 4400 5400
Wire Wire Line
	3700 4150 3600 4150
Connection ~ 4000 4650
Wire Wire Line
	4050 4650 4050 4800
Connection ~ 4050 4650
Wire Wire Line
	4950 2650 4950 3350
Wire Wire Line
	4950 3350 4900 3350
Wire Wire Line
	4900 3750 5050 3750
Wire Wire Line
	5050 3750 5050 4800
Connection ~ 5250 2750
Wire Wire Line
	5750 3450 4900 3450
Wire Wire Line
	5750 3000 5750 3450
Wire Wire Line
	5700 2750 5800 2750
Wire Wire Line
	5150 3950 4900 3950
Wire Wire Line
	4900 4150 5800 4150
Wire Wire Line
	5800 4150 5800 2750
Wire Wire Line
	5250 2500 5250 2800
Wire Wire Line
	5950 3750 5250 3750
Wire Wire Line
	5250 3750 5250 3550
Wire Wire Line
	5250 3550 4900 3550
Wire Wire Line
	4900 4250 5500 4250
Wire Wire Line
	5500 4250 5500 3850
Wire Wire Line
	5500 3850 5950 3850
$Comp
L C C3.1
U 1 1 50CECA5D
P 5500 3250
AR Path="/53A910B8/50CECA5D" Ref="C3.1"  Part="1" 
AR Path="/53A910B7/50CECA5D" Ref="C3.2"  Part="1" 
AR Path="/5391724D/50CECA5D" Ref="C3.3"  Part="1" 
AR Path="/53A910BA/50CECA5D" Ref="C3.4"  Part="1" 
AR Path="/53A910B9/50CECA5D" Ref="C3.5"  Part="1" 
F 0 "C3.1" H 5550 3350 50  0000 L CNN
F 1 "1n" H 5550 3150 50  0000 L CNN
F 2 "SM0805" H 5500 3250 60  0001 C CNN
F 3 "" H 5500 3250 60  0001 C CNN
	1    5500 3250
	0    1    -1   0   
$EndComp
$Comp
L R R6.1
U 1 1 50CECA5C
P 5500 3000
AR Path="/53A910B8/50CECA5C" Ref="R6.1"  Part="1" 
AR Path="/53A910B7/50CECA5C" Ref="R6.2"  Part="1" 
AR Path="/5391724D/50CECA5C" Ref="R6.3"  Part="1" 
AR Path="/53A910BA/50CECA5C" Ref="R6.4"  Part="1" 
AR Path="/53A910B9/50CECA5C" Ref="R6.5"  Part="1" 
F 0 "R6.1" V 5580 3000 50  0000 C CNN
F 1 "51k" V 5500 3000 50  0000 C CNN
F 2 "SM0603" H 5500 3000 60  0001 C CNN
F 3 "" H 5500 3000 60  0001 C CNN
	1    5500 3000
	0    1    -1   0   
$EndComp
$Comp
L GND #PWR030
U 1 1 50CECA5B
P 5250 3300
AR Path="/53A910B8/50CECA5B" Ref="#PWR030"  Part="1" 
AR Path="/53A910B7/50CECA5B" Ref="#PWR085"  Part="1" 
AR Path="/5391724D/50CECA5B" Ref="#PWR095"  Part="1" 
AR Path="/53A910BA/50CECA5B" Ref="#PWR0105"  Part="1" 
AR Path="/53A910B9/50CECA5B" Ref="#PWR0115"  Part="1" 
F 0 "#PWR0115" H 5250 3300 30  0001 C CNN
F 1 "GND" H 5250 3230 30  0001 C CNN
F 2 "" H 5250 3300 60  0001 C CNN
F 3 "" H 5250 3300 60  0001 C CNN
	1    5250 3300
	-1   0    0    -1  
$EndComp
$Comp
L GND #PWR031
U 1 1 50CEC8BA
P 3850 2700
AR Path="/53A910B8/50CEC8BA" Ref="#PWR031"  Part="1" 
AR Path="/53A910B7/50CEC8BA" Ref="#PWR086"  Part="1" 
AR Path="/5391724D/50CEC8BA" Ref="#PWR096"  Part="1" 
AR Path="/53A910BA/50CEC8BA" Ref="#PWR0106"  Part="1" 
AR Path="/53A910B9/50CEC8BA" Ref="#PWR0116"  Part="1" 
F 0 "#PWR0116" H 3850 2700 30  0001 C CNN
F 1 "GND" H 3850 2630 30  0001 C CNN
F 2 "" H 3850 2700 60  0001 C CNN
F 3 "" H 3850 2700 60  0001 C CNN
	1    3850 2700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR032
U 1 1 50CEC869
P 5250 2800
AR Path="/53A910B8/50CEC869" Ref="#PWR032"  Part="1" 
AR Path="/53A910B7/50CEC869" Ref="#PWR087"  Part="1" 
AR Path="/5391724D/50CEC869" Ref="#PWR097"  Part="1" 
AR Path="/53A910BA/50CEC869" Ref="#PWR0107"  Part="1" 
AR Path="/53A910B9/50CEC869" Ref="#PWR0117"  Part="1" 
F 0 "#PWR0117" H 5250 2800 30  0001 C CNN
F 1 "GND" H 5250 2730 30  0001 C CNN
F 2 "" H 5250 2800 60  0001 C CNN
F 3 "" H 5250 2800 60  0001 C CNN
	1    5250 2800
	-1   0    0    -1  
$EndComp
$Comp
L R R5.1
U 1 1 50CEC791
P 5500 2500
AR Path="/53A910B8/50CEC791" Ref="R5.1"  Part="1" 
AR Path="/53A910B7/50CEC791" Ref="R5.2"  Part="1" 
AR Path="/5391724D/50CEC791" Ref="R5.3"  Part="1" 
AR Path="/53A910BA/50CEC791" Ref="R5.4"  Part="1" 
AR Path="/53A910B9/50CEC791" Ref="R5.5"  Part="1" 
F 0 "R5.1" V 5580 2500 50  0000 C CNN
F 1 "51k" V 5500 2500 50  0000 C CNN
F 2 "SM0603" H 5500 2500 60  0001 C CNN
F 3 "" H 5500 2500 60  0001 C CNN
	1    5500 2500
	0    1    -1   0   
$EndComp
$Comp
L C C2.1
U 1 1 50CEC790
P 5500 2750
AR Path="/53A910B8/50CEC790" Ref="C2.1"  Part="1" 
AR Path="/53A910B7/50CEC790" Ref="C2.2"  Part="1" 
AR Path="/5391724D/50CEC790" Ref="C2.3"  Part="1" 
AR Path="/53A910BA/50CEC790" Ref="C2.4"  Part="1" 
AR Path="/53A910B9/50CEC790" Ref="C2.5"  Part="1" 
F 0 "C2.1" H 5550 2850 50  0000 L CNN
F 1 "1n" H 5550 2650 50  0000 L CNN
F 2 "SM0805" H 5500 2750 60  0001 C CNN
F 3 "" H 5500 2750 60  0001 C CNN
	1    5500 2750
	0    1    -1   0   
$EndComp
$Comp
L R R2.1
U 1 1 50CEB99A
P 4200 2650
AR Path="/53A910B8/50CEB99A" Ref="R2.1"  Part="1" 
AR Path="/53A910B7/50CEB99A" Ref="R2.2"  Part="1" 
AR Path="/5391724D/50CEB99A" Ref="R2.3"  Part="1" 
AR Path="/53A910BA/50CEB99A" Ref="R2.4"  Part="1" 
AR Path="/53A910B9/50CEB99A" Ref="R2.5"  Part="1" 
F 0 "R2.1" V 4280 2650 50  0000 C CNN
F 1 "0.33" V 4200 2650 50  0000 C CNN
F 2 "SM1210" H 4200 2650 60  0001 C CNN
F 3 "" H 4200 2650 60  0001 C CNN
	1    4200 2650
	0    -1   -1   0   
$EndComp
$Comp
L R R1.1
U 1 1 50CEB97A
P 4200 2450
AR Path="/53A910B8/50CEB97A" Ref="R1.1"  Part="1" 
AR Path="/53A910B7/50CEB97A" Ref="R1.2"  Part="1" 
AR Path="/5391724D/50CEB97A" Ref="R1.3"  Part="1" 
AR Path="/53A910BA/50CEB97A" Ref="R1.4"  Part="1" 
AR Path="/53A910B9/50CEB97A" Ref="R1.5"  Part="1" 
F 0 "R1.1" V 4280 2450 50  0000 C CNN
F 1 "0.33" V 4200 2450 50  0000 C CNN
F 2 "SM1210" H 4200 2450 60  0001 C CNN
F 3 "" H 4200 2450 60  0001 C CNN
	1    4200 2450
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR033
U 1 1 50AF2385
P 3300 3650
AR Path="/53A910B8/50AF2385" Ref="#PWR033"  Part="1" 
AR Path="/53A910B7/50AF2385" Ref="#PWR088"  Part="1" 
AR Path="/5391724D/50AF2385" Ref="#PWR098"  Part="1" 
AR Path="/53A910BA/50AF2385" Ref="#PWR0108"  Part="1" 
AR Path="/53A910B9/50AF2385" Ref="#PWR0118"  Part="1" 
F 0 "#PWR0118" H 3300 3740 20  0001 C CNN
F 1 "+5V" H 3300 3740 30  0000 C CNN
F 2 "" H 3300 3650 60  0001 C CNN
F 3 "" H 3300 3650 60  0001 C CNN
	1    3300 3650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR034
U 1 1 50AF2313
P 4050 4800
AR Path="/53A910B8/50AF2313" Ref="#PWR034"  Part="1" 
AR Path="/53A910B7/50AF2313" Ref="#PWR089"  Part="1" 
AR Path="/5391724D/50AF2313" Ref="#PWR099"  Part="1" 
AR Path="/53A910BA/50AF2313" Ref="#PWR0109"  Part="1" 
AR Path="/53A910B9/50AF2313" Ref="#PWR0119"  Part="1" 
F 0 "#PWR0119" H 4050 4800 30  0001 C CNN
F 1 "GND" H 4050 4730 30  0001 C CNN
F 2 "" H 4050 4800 60  0001 C CNN
F 3 "" H 4050 4800 60  0001 C CNN
	1    4050 4800
	1    0    0    -1  
$EndComp
$Comp
L C C1.1
U 1 1 50AF1E60
P 5050 5000
AR Path="/53A910B8/50AF1E60" Ref="C1.1"  Part="1" 
AR Path="/53A910B7/50AF1E60" Ref="C1.2"  Part="1" 
AR Path="/5391724D/50AF1E60" Ref="C1.3"  Part="1" 
AR Path="/53A910BA/50AF1E60" Ref="C1.4"  Part="1" 
AR Path="/53A910B9/50AF1E60" Ref="C1.5"  Part="1" 
F 0 "C1.1" H 5100 5100 50  0000 L CNN
F 1 "68n" H 5100 4900 50  0000 L CNN
F 2 "SM0805" H 5050 5000 60  0001 C CNN
F 3 "" H 5050 5000 60  0001 C CNN
	1    5050 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR035
U 1 1 50AF1E09
P 4850 5450
AR Path="/53A910B8/50AF1E09" Ref="#PWR035"  Part="1" 
AR Path="/53A910B7/50AF1E09" Ref="#PWR090"  Part="1" 
AR Path="/5391724D/50AF1E09" Ref="#PWR0100"  Part="1" 
AR Path="/53A910BA/50AF1E09" Ref="#PWR0110"  Part="1" 
AR Path="/53A910B9/50AF1E09" Ref="#PWR0120"  Part="1" 
F 0 "#PWR0120" H 4850 5450 30  0001 C CNN
F 1 "GND" H 4850 5380 30  0001 C CNN
F 2 "" H 4850 5450 60  0001 C CNN
F 3 "" H 4850 5450 60  0001 C CNN
	1    4850 5450
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR036
U 1 1 50AF1DE3
P 4400 5400
AR Path="/53A910B8/50AF1DE3" Ref="#PWR036"  Part="1" 
AR Path="/53A910B7/50AF1DE3" Ref="#PWR091"  Part="1" 
AR Path="/5391724D/50AF1DE3" Ref="#PWR0101"  Part="1" 
AR Path="/53A910BA/50AF1DE3" Ref="#PWR0111"  Part="1" 
AR Path="/53A910B9/50AF1DE3" Ref="#PWR0121"  Part="1" 
F 0 "#PWR0121" H 4400 5490 20  0001 C CNN
F 1 "+5V" H 4400 5490 30  0000 C CNN
F 2 "" H 4400 5400 60  0001 C CNN
F 3 "" H 4400 5400 60  0001 C CNN
	1    4400 5400
	-1   0    0    1   
$EndComp
$Comp
L R R3.1
U 1 1 50AF1C25
P 4400 5050
AR Path="/53A910B8/50AF1C25" Ref="R3.1"  Part="1" 
AR Path="/53A910B7/50AF1C25" Ref="R3.2"  Part="1" 
AR Path="/5391724D/50AF1C25" Ref="R3.3"  Part="1" 
AR Path="/53A910BA/50AF1C25" Ref="R3.4"  Part="1" 
AR Path="/53A910B9/50AF1C25" Ref="R3.5"  Part="1" 
F 0 "R3.1" V 4480 5050 50  0000 C CNN
F 1 "22k" V 4400 5050 50  0000 C CNN
F 2 "SM0603" H 4400 5050 60  0001 C CNN
F 3 "" H 4400 5050 60  0001 C CNN
	1    4400 5050
	1    0    0    -1  
$EndComp
$Comp
L L6208N DD2.1
U 1 1 50AE2A4D
P 4300 3750
AR Path="/53A910B8/50AE2A4D" Ref="DD2.1"  Part="1" 
AR Path="/53A910B7/50AE2A4D" Ref="DD2.2"  Part="1" 
AR Path="/5391724D/50AE2A4D" Ref="DD2.3"  Part="1" 
AR Path="/53A910BA/50AE2A4D" Ref="DD2.4"  Part="1" 
AR Path="/53A910B9/50AE2A4D" Ref="DD2.5"  Part="1" 
F 0 "DD2.1" H 3800 4600 50  0000 L BNN
F 1 "L6208N" H 4300 2850 50  0000 L BNN
F 2 "DIP-24__300_ELL" H 4300 3900 50  0001 C CNN
F 3 "" H 4300 3750 60  0001 C CNN
	1    4300 3750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR037
U 1 1 534B86E7
P 3650 3750
AR Path="/53A910B8/534B86E7" Ref="#PWR037"  Part="1" 
AR Path="/53A910B7/534B86E7" Ref="#PWR092"  Part="1" 
AR Path="/5391724D/534B86E7" Ref="#PWR0102"  Part="1" 
AR Path="/53A910BA/534B86E7" Ref="#PWR0112"  Part="1" 
AR Path="/53A910B9/534B86E7" Ref="#PWR0122"  Part="1" 
F 0 "#PWR0122" H 3650 3750 30  0001 C CNN
F 1 "GND" H 3650 3680 30  0001 C CNN
F 2 "" H 3650 3750 60  0000 C CNN
F 3 "" H 3650 3750 60  0000 C CNN
	1    3650 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3650 3650 3650
Wire Wire Line
	3650 3650 3650 3750
Wire Wire Line
	3500 3550 3500 3900
Wire Wire Line
	3600 4150 3600 3900
Wire Wire Line
	3600 3900 3500 3900
Wire Wire Line
	4850 5300 4850 5450
$Comp
L R R4.1
U 1 1 534BB108
P 4700 5050
AR Path="/53A910B8/534BB108" Ref="R4.1"  Part="1" 
AR Path="/53A910B7/534BB108" Ref="R4.2"  Part="1" 
AR Path="/5391724D/534BB108" Ref="R4.3"  Part="1" 
AR Path="/53A910BA/534BB108" Ref="R4.4"  Part="1" 
AR Path="/53A910B9/534BB108" Ref="R4.5"  Part="1" 
F 0 "R4.1" V 4780 5050 40  0000 C CNN
F 1 "1.8k" V 4707 5051 40  0000 C CNN
F 2 "SM0603" V 4630 5050 30  0001 C CNN
F 3 "~" H 4700 5050 30  0000 C CNN
	1    4700 5050
	1    0    0    -1  
$EndComp
Text HLabel 2700 2300 0    60   Input ~ 0
+10V
Text HLabel 5950 3550 2    60   Output ~ 0
A
Text HLabel 5950 3650 2    60   Output ~ 0
A*
Text HLabel 5950 3750 2    60   Output ~ 0
B
Text HLabel 5950 3850 2    60   Output ~ 0
B*
$Comp
L +5V #PWR038
U 1 1 5396FC14
P 2800 2700
AR Path="/53A910B8/5396FC14" Ref="#PWR038"  Part="1" 
AR Path="/53A910B7/5396FC14" Ref="#PWR093"  Part="1" 
AR Path="/5391724D/5396FC14" Ref="#PWR0103"  Part="1" 
AR Path="/53A910BA/5396FC14" Ref="#PWR0113"  Part="1" 
AR Path="/53A910B9/5396FC14" Ref="#PWR0123"  Part="1" 
F 0 "#PWR0123" H 2800 2790 20  0001 C CNN
F 1 "+5V" H 2800 2790 30  0000 C CNN
F 2 "" H 2800 2700 60  0001 C CNN
F 3 "" H 2800 2700 60  0001 C CNN
	1    2800 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2700 2800 2750
Text HLabel 2700 2750 0    60   Input ~ 0
SM_5V
Text HLabel 2700 3350 0    60   Input ~ 0
SM_CLK
Text HLabel 2700 3450 0    60   Input ~ 0
SM_DIR
Text HLabel 2750 4050 0    60   Input ~ 0
SM_EN
Wire Wire Line
	2800 2750 2700 2750
Wire Wire Line
	2700 2300 5150 2300
Wire Wire Line
	5150 2300 5150 3950
Wire Wire Line
	2700 3150 2700 2900
$Comp
L GND #PWR039
U 1 1 53A16DEA
P 2700 3150
AR Path="/53A910B8/53A16DEA" Ref="#PWR039"  Part="1" 
AR Path="/53A910B7/53A16DEA" Ref="#PWR094"  Part="1" 
AR Path="/5391724D/53A16DEA" Ref="#PWR0104"  Part="1" 
AR Path="/53A910BA/53A16DEA" Ref="#PWR0114"  Part="1" 
AR Path="/53A910B9/53A16DEA" Ref="#PWR0124"  Part="1" 
F 0 "#PWR0124" H 2700 3150 30  0001 C CNN
F 1 "GND" H 2700 3080 30  0001 C CNN
F 2 "" H 2700 3150 60  0001 C CNN
F 3 "" H 2700 3150 60  0001 C CNN
	1    2700 3150
	1    0    0    -1  
$EndComp
Text HLabel 2700 2900 0    60   Input ~ 0
PWR_GND
Wire Wire Line
	4450 2450 5000 2450
Wire Wire Line
	4450 2650 4950 2650
Wire Wire Line
	3950 2450 3950 2650
Wire Wire Line
	3850 2700 3850 2650
Wire Wire Line
	3850 2650 3950 2650
Connection ~ 3950 2650
Text HLabel 2700 2500 0    60   Input ~ 0
+24V
Wire Wire Line
	2700 2500 3500 2500
Wire Wire Line
	3500 2500 3500 3150
Wire Wire Line
	3300 3650 3500 3650
Connection ~ 3500 3650
Wire Wire Line
	2700 3350 3700 3350
Wire Wire Line
	2700 3450 3700 3450
Wire Wire Line
	3900 4650 4200 4650
NoConn ~ 3700 3050
$EndSCHEMATC

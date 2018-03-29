Measuring Frequency of Clock Pulse using TM4C123GH6PM(tiva-c microcontroller)
TM4C123G:

The Tiva-C (a.k.a. TM4C) LaunchPads are inexpensive self-contained, single-board microcontrollers, about the size of a credit card, featuring an ARM Cortex-M4F 32-bit CPU operating at 80 to 120 MHz, manufactured by Texas Instruments.
PROJECT:

In this project, the tm4c123 microcontroller is used for measuring the frequency of an input clock pulse (0v-3.3v).

PB(7:0) (portB) are used as data lines for interfacing 4 seven segment displays.  

PC(7:4)(portC 4 pins) are used to select the seven segment display for each digit. All the four displays are time multiplexed.

PD2 is the input pin for clock pulse whose frequency has to be measured. 

Please ensure that clock pulse voltage ranges from 0v to 3.3v as the tm4c123g microcontroller operates on 3.3v.
Also the frequency should be scaled to 4 digits as there are only 4 seven segment displays.

Tm4c123gh6pm datasheet:
http://www.ti.com/lit/ds/symlink/tm4c123gh6pm.pdf

TivaWare Peripheral Driver library guide:
http://www.ti.com/lit/ug/spmu298d/spmu298d.pdf


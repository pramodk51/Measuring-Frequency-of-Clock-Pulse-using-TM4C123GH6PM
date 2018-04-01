/* the input frequency should be less than 9.9KHz(4 DIGITS) only 4 seven segments displays are used.
 * otherwise you will have to modify the scale in line 110
 *
 * */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"



void PortDInt_Handler(void);     // interrupt handler for GPIO portD(INPUT CLOCK)
void Timer5AInt_Handler(void);   //timer5A interrupt handler
uint32_t data[2];               //count of timer stored in this array


int i=0;   //check line 142(index of buffer)
int n=0;  //counting the no. of timeouts of timer
int f;    //variable for frequency

int ssdnum[16]={
                 0b11111100,0b01100000,0b11011010,0b11110010,
                 0b01100110,0b10110110,0b10111110,0b11100000,
                 0b11111110,0b11110110,0b11101110,0b00111110,
                 0b10011100,0b01111010,0b10011110,0b10001110
                 };                     //look up table for seven segment display

uint32_t ssdDig[4]={GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7};  //selecting the seven segment display (4 seven segment display)

int main(void){

     SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);   //40MHz clock set (200/5=40)


     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);                // enabling GPIO portD
     //
     // Wait for the GPIOD module to be ready.
     //
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD))
     { }



     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);                // enabling GPIO portB
     //
     // Wait for the GPIOB module to be ready.
     //
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
     { }


     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);                // enabling GPIO portC
     //
     // Wait for the GPIOC module to be ready.
     //
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC))
     { }


     GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,0xFF);                // all pins of portB as output (data lines for seven segment display)
     GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,0xF0);                // pin 4,5,6 and 7 of portC as output(for selecting the seven segment display)
     GPIOPinTypeGPIOInput(GPIO_PORTD_BASE,GPIO_PIN_2);           // pin 2 of portD as input


     SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);               //enabling peripheral timer5


     //
     // Wait for the Timer5 module to be ready.
     //
     while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER5))
     { }



       GPIOIntRegister(GPIO_PORTD_BASE,PortDInt_Handler);             //interrupt from GPIO pin connected to input clock
                                                                       //(whose frequency has to be measured)
       GPIOIntClear(GPIO_PORTD_BASE,GPIO_PIN_2);                      //clear any interrupt on pin2
       GPIOIntTypeSet(GPIO_PORTD_BASE,GPIO_PIN_2,GPIO_FALLING_EDGE); //interrupt on every falling edge
       GPIOIntEnable(GPIO_PORTD_BASE,GPIO_PIN_2);                    //enable the GPIO interrupt source

       TimerClockSourceSet(TIMER5_BASE,TIMER_CLOCK_SYSTEM);        //use system clock as the timer clock
       TimerDisable(TIMER5_BASE,TIMER_A);                         //disable timer before configuring
       TimerConfigure(TIMER5_BASE,TIMER_CFG_PERIODIC);          //32-bit timer
       TimerLoadSet(TIMER5_BASE,TIMER_A,500000000);                  //load for timer.


       TimerIntRegister(TIMER5_BASE,TIMER_A,Timer5AInt_Handler);   //timer5A interrupt
       TimerIntClear(TIMER5_BASE, INT_TIMER5A_TM4C123);           //clear any interrupt on Timer5A
       TimerIntEnable(TIMER5_BASE,TIMER_TIMA_TIMEOUT);           //interrupt on Timer5A timeout



       TimerEnable(TIMER5_BASE,TIMER_A);                        //start timer5A
       IntEnable(INT_GPIOD);                                     //enable for GPIO portD
       IntMasterEnable();                                       //Master interrupt enable



    while(1)
    {


        f=(40*1000000)/(data[0]+(n*500000000)-data[1]);  //calculation of frequency in Hz     (40MHz clock used)
        int temp2=f;                                     //copy value of frequency to temp2
        int temp, k;

/*display*/

                  for(k=0;k<4;k++)
                      {
                         temp=f%10;           //remainder is the digit required
                                              //(frequency input should be less than 9KHz  as only four seven segment display are used  )

                         GPIOPinWrite(GPIO_PORTB_BASE,0XFF,ssdnum[temp]); //writing binary value corresponding to digits
                         GPIOPinWrite(GPIO_PORTC_BASE,0XF0,ssdDig[k]);  //pin 4 of PORTC corresponds to once digit.
                         SysCtlDelay(20000);
                         GPIOPinWrite(GPIO_PORTC_BASE,0XF0,0);           //clear portC.
                          f = f/10;                            //divide by 10 to retrieve next digits each time

                          if(k==3)
                         {
                           f=temp2;    //if all digits are shown once ,again assign the f with original value stored in temp2
                         }

                      }
       }



return 0;
}

void PortDInt_Handler()
{
    GPIOIntClear(GPIO_PORTD_BASE,GPIO_PIN_2);  //firstly, clear the interrupt
    data[i]=TimerValueGet(TIMER5_BASE,TIMER_A); //get the value of timer at falling edge.
    i++;
    if(i==2)
    {  TimerDisable(TIMER5_BASE,TIMER_A);    //once two values are found,disable all interrupts
       GPIOIntDisable(GPIO_PORTD_BASE,GPIO_PIN_2);
       IntDisable(INT_GPIOD);
       IntMasterDisable();
     }

      }

void Timer5AInt_Handler()
{
    TimerIntClear(TIMER5_BASE, INT_TIMER5A_TM4C123);
    n++;     //increase when timeout
}

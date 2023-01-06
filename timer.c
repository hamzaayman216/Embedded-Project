#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "tm4c123gh6pm.h"
#include "LCD.h"
#include "DIO.h"
#include "inttypes.h""

// This function converts an integer representing time in timer ticks to time in seconds.

int PeriodTosecTimer(uint64_t x){
  return (x / 15999999);
}

// This function converts an integer representing time in seconds to time in timer ticks.

int secToTicksTimer(uint64_t sec){
   return sec*16000000-1;
}

// This function is called when the Timer1 interrupt occurs.
// It displays the current time in Timer0 on the LCD screen and prints it to the console.

void Timer1_ISR_Timer(){
  int x = PeriodTosecTimer(TimerValueGet64(WTIMER0_BASE));
  TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
  LCD_printInt_Timer_Stopwatch(x);
  
  if (x!=0)
    LCD_command(0x80);
}

// This function is the interrupt handler for Timer0. It disables the interrupt for Timer1,
// clears the interrupt flag for Timer0, sets the output pin on port A high to enable the buzzer, waits for 2 seconds, 
// then sets the output pin on port A high to disable the buzzer

void Timer0_ISR_Timer(){
  TimerIntDisable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
  TimerIntClear(WTIMER0_BASE,TIMER_TIMA_TIMEOUT);
  GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2); //turn on the buzzer when the timer reaches 00:00
  delayMs(2000); // allow the buzzer to make noise for only 2 seconds
  GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0); //turn off the buzzer
  
}

// This function initializes and configures Timer0 and Timer1, and sets up the interrupt handlers

void timer_init (uint64_t s, uint64_t m) {
  
  uint64_t Time = (((m*60) + s) * 16000000) -1 ;
  
  LCD_init ();
  
  // Enable the GPIOA peripheral and wait for it to be ready.
  
  SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
  while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOA ));
  GPIOPinTypeGPIOOutput (GPIO_PORTA_BASE , GPIO_PIN_2 ) ; // Set the pin on port A as an output pin.
  
  GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0); // Set the output pin on port A to low.
  
  // Enable the TIMER0 peripheral and wait for it to be ready.
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WTIMER0));
  TimerConfigure(WTIMER0_BASE,TIMER_CFG_ONE_SHOT); // Configure TIMER0 as a one-shot timer.
  TimerLoadSet64(WTIMER0_BASE,(Time)); // Load the value of Time into TIMER0.
  TimerIntEnable(WTIMER0_BASE,TIMER_TIMA_TIMEOUT); // Enable the timeout interrupt for TIMER0.
  TimerIntRegister(WTIMER0_BASE,TIMER_BOTH,Timer0_ISR_Timer); // Set the interrupt handler for TIMER0 to Timer0_ISR_Timer.
  TimerEnable(WTIMER0_BASE,TIMER_BOTH); // Enable TIMER0.

  // Enable the TIMER1 peripheral and wait for it to be ready.
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));
  TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC); // Configure TIMER1 as a periodic timer.
  TimerLoadSet(TIMER1_BASE,TIMER_BOTH,15999999); // Load the value of one second into TIMER1.
  TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT); // Enable the timeout interrupt for TIMER1.
  TimerIntRegister(TIMER1_BASE,TIMER_BOTH,Timer1_ISR_Timer); // Set the interrupt handler for TIMER1 to Timer1_ISR_Timer.
  TimerEnable(TIMER1_BASE,TIMER_BOTH); // Enable TIMER1.
  
  
  while(1) {
    
    // If the button on port A, pin 4 is pressed, disable the interrupt for TIMER1,
    // clear the LCD screen, display the menu for three modes, 
    // and return from the function to wait for the user input once more to enter any of the three modes
    
    if (DIO_ReadPin(&GPIO_PORTA_DATA_R,4) == 1){
      
      TimerIntDisable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
      
      LCD_command(0x01); // Clear LCD screen
      LCD_printString("Mode : 1.Calc");
      LCD_command(0xC0);
      LCD_printString("2.Timer 3.SW");
      
      return;
    
    
    
    }
    
  }
}
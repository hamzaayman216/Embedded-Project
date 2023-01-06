#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
#include <stdlib.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
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

// This function converts an integer representing time in timer ticks to time in seconds.

int PeriodTosec(uint64_t x){
  return (x / 15999999) ;
}

// This function converts an integer representing time in seconds to time in timer ticks.

int secToTicks(uint64_t sec){
    return sec*16000000-1;
}

// This function is called when the Timer1 interrupt occurs.
// It displays the current time in Timer0 on the LCD screen and prints it to the console.

void Timer1_ISR(){
  int x = PeriodTosec(TimerValueGet64(WTIMER0_BASE));
  
  TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
  LCD_printInt_Timer_Stopwatch(x);
  printf("%d", x);
  //delayMs(900);
  LCD_command(0x80);

}

// This function is called when the Timer0 interrupt occurs.
// It disables the Timer1 interrup and clears the Timer0 interrupt

void Timer0_ISR(){
  TimerIntDisable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
  TimerIntClear(WTIMER0_BASE,TIMER_TIMA_TIMEOUT);
}

// This function pauses and resumes the timer.

void pause(){
  static int x=0;
  if (x==0){ 
    
      // Disable both timer0 and timer1.
 
      TimerDisable(WTIMER0_BASE,TIMER_BOTH);
      TimerDisable(TIMER1_BASE,TIMER_BOTH);
      
      // Display the current time on the LCD screen.
      
      LCD_command(0x80);
      LCD_printInt_Timer_Stopwatch(PeriodTosec(WTIMER0_TAR_R));
      x++; 

      
  }
  else{
    
      // Enable both timer0 and timer1.
    
      TimerEnable(WTIMER0_BASE,TIMER_BOTH);
      TimerEnable(TIMER1_BASE,TIMER_BOTH);      
      LCD_command(0x80);
      x--;
      
  }
  
  delayMs(200);
  
  GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0); // Clear the GPIO interrupt.

}

// This function initializes the stopwatch.

void stopwatch_init() {
  
  //TIMER0_TAR_R=0;
  //TIMER0_TAV_R=0;
  
  // Enable the GPIO peripheral and set it up as input.
  
  SYSCTL_RCGCGPIO_R |= 0X20;
  while((SYSCTL_PRGPIO_R & 0X20)==0);
  GPIO_PORTF_CR_R|= 0X11;
  GPIO_PORTF_DIR_R|= 0X00;
  GPIO_PORTF_LOCK_R=0x4c4f434b;
  GPIO_PORTF_PUR_R |= 0x11;
  GPIO_PORTF_DEN_R|= 0X11;
    
  GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0); // Enable the interrupt on the PF0 pin.
  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_RISING_EDGE); // Set the interrupt type on the PF0 pin to rising edge.
  GPIOIntRegisterPin(GPIO_PORTF_BASE,GPIO_INT_PIN_0,pause); // Register the interrupt handler for the PF0 pin.
  
  LCD_init();
  
  // Enable the timer0 and wait for it to be ready.
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WTIMER0));
  TimerConfigure(WTIMER0_BASE,TIMER_CFG_ONE_SHOT_UP); // Configure timer0 as a one-shot up-counter and timer1 as a periodic timer.
  TimerLoadSet64(WTIMER0_BASE, 9223372036854775807);
  TimerIntEnable(WTIMER0_BASE,TIMER_TIMA_TIMEOUT); // Enable the timer interrupts for timer0 
  TimerIntRegister(WTIMER0_BASE,TIMER_BOTH,Timer0_ISR); // Register the interrupt handlers for timer0.
  TimerEnable(WTIMER0_BASE,TIMER_BOTH); // Enable timer0
  
  // Enable the timer1 and wait for it to be ready.
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1));
  TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC); // Configure timer1 as a periodic timer.
  TimerLoadSet(TIMER1_BASE,TIMER_BOTH,15999999);
  TimerEnable(TIMER1_BASE,TIMER_BOTH); // Enable timer1.
  TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT); // Enable the timer interrupts for timer1.
  TimerIntRegister(TIMER1_BASE,TIMER_BOTH,Timer1_ISR); // Register the interrupt handlers for timer1.
   
  WTIMER0_TAR_R=0;
  WTIMER0_TAV_R=0;
  
  while(1) {
    
         // Check if the button on pin 0 of port F is pressed.
    
         if ((DIO_ReadPin(&GPIO_PORTF_DATA_R,0)) == 0)
              pause(); // Call the pause function to pause the timer.
         
         // Check if the button on pin 4 of port F is pressed.
         
         if ((DIO_ReadPin(&GPIO_PORTF_DATA_R,4)) == 0){
           
             // Reset the timer registers to 0.

             WTIMER0_TAR_R=0;
             WTIMER0_TAV_R=0;
         }
         
         // Check if the button on pin 4 of port A is pressed.
             
         if ((DIO_ReadPin(&GPIO_PORTA_DATA_R,4)) == 1) {
           
            // Disable the Timer1 interrupt.
           
            TimerIntDisable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
            
            //display the menu to choose modes from once again
            
            LCD_command(0x01); // Clear LCD screen
            LCD_printString("Mode : 1.Calc");
            LCD_command(0xC0);
            LCD_printString("2.Timer 3.SW");
            
            return; //return back to the while loop in the main function to wait for input from the user to enter the mode desired
    }
  
  }
  
}
  
  
  
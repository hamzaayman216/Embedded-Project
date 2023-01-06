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
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "tm4c123gh6pm.h"
#include "keypad.h"
#include "DIO.h"
#include "LCD.h"
#include "stopwatch.h"
#include "timer.h"
#include "calculator.h"
#include <string.h>


int main()
{ 
  __asm("CPSIE I");
  LCD_init(); // Initialize LCD screen
  
  // Enable and configure GPIO Port F
  
  SYSCTL_RCGCGPIO_R |= 0X20;
  while((SYSCTL_PRGPIO_R & 0X20)==0);
  GPIO_PORTF_CR_R|= 0X31;
  GPIO_PORTF_DIR_R|= 0X00;
  GPIO_PORTF_LOCK_R=0x4c4f434b;
  GPIO_PORTF_PUR_R |= 0x31;
  GPIO_PORTF_DEN_R|= 0X31;
  
  // Enable and configure GPIO Port A
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
  GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,(GPIO_PIN_3 | GPIO_PIN_4 ));
  GPIO_PORTA_PDR_R |= 0x18;

   // Display menu on LCD screen
     
   LCD_command(0x01);
   LCD_printString("Mode : 1.Calc");
   LCD_command(0xC0);
   LCD_printString("2.Timer 3.SW");
  
 
  // Wait for user input
  
  while(1){ 
  
    // If user selects calculator mode
      
    if((DIO_ReadPin(&GPIO_PORTF_DATA_R,0)) == 0){
      LCD_command(0x01);  // Clear LCD screen
      KeyPad_Init();  // Initialize keypad
      LCD_printString("Enter eq: "); // Prompt user to enter equation
      calculator_init();  // Initialize calculator mod
    }
    
    // If user selects timer mode
    
     if((DIO_ReadPin(&GPIO_PORTF_DATA_R,4)) == 0){
       
       int min;
       int sec;
       char mins[20] = "0";
       char secs[20] = "0";
       char charSec_returned;
       char charMin_returned;
 
      KeyPad_Init(); // Initialize keypad
      LCD_command(0x01); // Clear LCD screen
      LCD_printString("Enter mins :"); // Prompt user to enter minutes
      
      // Read minutes input from
      
      while (1) {
        
        charMin_returned = Keypad_read();  // Read input from keypad
        
        if ( charMin_returned == '=') { // If user is done entering minutes
          
          break; // Exit loop
          
        }
        
        else // If user is still entering minutes
          strncat(mins, &charMin_returned, 1);  // Add input to mins string
      
      }
      
      min = atoi(mins); // Convert mins string to integer
      
      LCD_command(0xC0);  // Move cursor to second line
      LCD_printString("Enter secs :");  // Prompt user to enter seconds
      
      // Read seconds input from keypad 
      
       while (1) {
        
        char charSec_returned = Keypad_read(); // Read input from keypad
        
        if (charSec_returned == '=') { // If user is done entering seconds
          
          break; // Exit loop
          
        }
        
        else // If user is still entering seconds 
          strncat(secs, &charSec_returned, 1); // Add input to secs string
       
      }
      
      sec = atoi(secs); // Convert secs string to integer
      
      
      
      timer_init(sec,min); // Initialize timer with user input value
     

    }
    
    // If user selects stopwatch mode
  
    if (DIO_ReadPin(&GPIO_PORTA_DATA_R,3) == 1){
      
      LCD_command(0x01); // Clear LCD screen
      stopwatch_init(); // Initialize stopwatch mode
     
    }
  }
    
  return 0;
}
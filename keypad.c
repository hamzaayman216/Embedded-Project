#include "stdio.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "bitwise_operation.h"
#include "DIO.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "LCD.h"


char arr[4][4]={{'1','2','3','+'},
               {'4','5','6','-'},
               {'7','8','9','/'},
               {'*','0','#','='}};


void KeyPad_Init (void)


{
      
      SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOE);
      while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOE ));
      GPIOPinTypeGPIOOutput (GPIO_PORTE_BASE , (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3));
      
      SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOC);
      while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOC ));
      GPIOPinTypeGPIOInput (GPIO_PORTC_BASE , (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7));
                            

}


char Keypad_read (void) {
  char key = '\0';  // Initialize key to null character
  
  while (1) {
    for (int t=0; t<4; t++) {
      DIO_ClearPort (&GPIO_PORTE_DATA_R);
      DIO_WritePin (&GPIO_PORTE_DATA_R, t,1);
      
      if ((DIO_ReadPort(&GPIO_PORTC_DATA_R) & 0xF0) == 0x10){
        key = arr[t][0];
        break;
      }
      if ((DIO_ReadPort(&GPIO_PORTC_DATA_R) & 0xF0) == 0x20){
        key = arr[t][1];
        break;
      }
      if ((DIO_ReadPort(&GPIO_PORTC_DATA_R) & 0xF0) == 0x40){
        key = arr[t][2];
        break;
      }
      if ((DIO_ReadPort(&GPIO_PORTC_DATA_R) & 0xF0) == 0x80){
        key = arr[t][3];
        break;
      }
    }
    
    if (key != '\0') {  // If a key was pressed
      LCD_data(key);
      printf("%c" ,key);
      SysCtlDelay(SysCtlClockGet() / 3);  // Delay for a while before another key can be pressed
      break;  // Exit loop
    }
  }
  
  return key;
}



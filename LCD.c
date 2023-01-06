#include "LCD.h"

// Initialize the LCD screen

void LCD_init (void)
{

SYSCTL_RCGCGPIO_R |= 0x01;
SYSCTL_RCGCGPIO_R |= 0x02;
GPIO_PORTA_DIR_R |= 0xE0;
GPIO_PORTA_DEN_R |= 0xE0;
GPIO_PORTB_DIR_R  = 0xFF;
GPIO_PORTB_DEN_R  = 0xFF;
  
delayMs (20);
LCD_command (0x30) ;
delayMs (5);
LCD_command (0x30) ;
delayUs (100);
LCD_command (0x38) ; 
LCD_command (0x06) ;
LCD_command (0x01) ; 
LCD_command (0x0F) ; 
}

// Send a command to the LCD screen

void LCD_command (uint8 command)
{
GPIO_PORTA_DATA_R=0;
GPIO_PORTB_DATA_R=command;
GPIO_PORTA_DATA_R=EN;
    delayUs(0);
GPIO_PORTA_DATA_R=0;

 // If the command is a clear screen or return home command, delay for 2 ms
if (command<4)
{
  delayMs(2);
}
else
{
  delayUs(40);
}
}

// Send a character to the LCD screen

void LCD_data(uint8 data)
{
  GPIO_PORTA_DATA_R=RS;
  GPIO_PORTB_DATA_R=data;
  GPIO_PORTA_DATA_R=(EN | RS) ;
  delayUs(0);
  GPIO_PORTA_DATA_R=0;
  delayUs(40);
}

// Delay for a specified number of milliseconds

void delayMs(uint32 n)
{
  int i,j;
  for(i=0;i<n;i++)
  {
    for(j=0;j<3180;j++)
    {}
  }
}

// Delay for a specified number of milliseconds

void delayUs(uint32 n)
{
  int i,j;
  for(i=0;i<n;i++)
  {
    for(j=0;j<3;j++)
    {}
  }
}

// Print an integer to the LCD screen

void LCD_printInt(uint32 no)
{
  char toprint[4] = {0};
  sprintf(toprint, "%d", no);
  int i = 0;
  while(toprint[i] != '\0')
  {
    LCD_data(toprint[i]);
    i++;
  }
}

// Print an integer in the form of minutes and seconds to the LCD screen

void LCD_printInt_Timer_Stopwatch(uint32 no)
{
  char toprint[10] = {0};

  // Calculate the number of minutes and seconds
  int minutes = no / 60;
  int seconds = no % 60;

  // Format the string to print
  sprintf(toprint, "%02d:%02d", minutes, seconds);

  // Print the string to the LCD
  int i = 0;
  while(toprint[i] != '\0')
  {
    LCD_data(toprint[i]);
    i++;
  }
}

// Print a floating point number to the LCD screen


void LCD_printFloat(float no)
{
  char toprint[10] = {0};
  sprintf(toprint, "%f", no);
  int i = 0;
  while(toprint[i] != '\0')
  {
    LCD_data(toprint[i]);
    i++;
  }
}

// Print a string to the LCD screen

void LCD_printString(char* str)
{
  int i = 0;
// Send each character of the string to the LCD screen
while(str[i] != '\0') {
LCD_data(str[i]);
i++;
}
}

#include "keypad.h"
#include "LCD.h"
#include "string.h"
#include "DIO.h"

// This function takes in two floating point numbers and an operator, 
// performs the operation and prints the result on the LCD screen.

void perform_operation (float num1, float num2, char operator ) {
  
  switch (operator) {
    
    // If operator is '+', perform addition
    
    case '+':
      
      LCD_command(0xC0);
      LCD_printFloat(num1+num2);
      break;
      
    // If operator is '-', perform subtraction
      
    case '-':
      
      LCD_command(0xC0);
      LCD_printFloat(num1-num2);
      break;
    
    // If operator is '/', perform division
      
    case '/':
      LCD_command(0xC0);
      LCD_printFloat(num1/num2);
      break;
      
    // If operator is '*', perform multiplication
      
    case '*':
      LCD_command(0xC0);
      LCD_printFloat(num1*num2);
      break;
  }
}
  
// This function initializes the calculator mode of the program

void calculator_init(){
  
  
char char_returned;
char char_returned2;
char string1[20] = "0";
char string2[20] = "0";
float num1;
float num2;
char operator;

while (1) {
  
    // Loop indefinitely until a valid equation is entered

    while(1){
  
      char_returned = Keypad_read(); // Read a character from the keypad
      
      if ((char_returned == '+') || (char_returned == '-') || (char_returned == '*') || (char_returned == '/')) { 
        
        operator = char_returned; // If the character returned is an operator (+, -, *, or /), store it in the 'operator' variable
        
        while (1) {
          
          char_returned2 = Keypad_read();  // Read a character from the keypad 
          
          if(char_returned2==('+') || char_returned2==('-') || char_returned2==('*') || char_returned2==('/')){ //if two operators are read from keypad after each other display error message
            LCD_command(0xc0);
            LCD_printString("ERROR");
            
            delayMs(2000); //wait 2 seconds after the error message is displayed and then go back to displaying the menus
            
            LCD_command(0x01); // Clear LCD screen
            LCD_printString("Mode : 1.Calc");
            LCD_command(0xC0);
            LCD_printString("2.Timer 3.SW");
            
            return;
            
            
          }
          
          // if the input is the equal sign, exit the loop to perform calculation
          
          if (char_returned2 == ( '=')) {
        
             break;
          
          }
          
          else {
            
            strncat(string2, &char_returned2, 1); //conactenate the character returned to the variable string2 to collect the second number of equation
              
          }
                  
        }
        
        break;
        
      }
      
      else {
        
        strncat(string1, &char_returned, 1); //conactenate the character returned to the variable string1 to collect the first number of equation
         
      }
  }
      
      num1 = atof(string1); //convert the string of the first number to a float number
      num2 = atof(string2); //convert the string of the second number to a float number
      LCD_command(0xC0);
      perform_operation(num1, num2, operator); //calculate the equation given by calling the perform_operation function
      
      while (1) {
        
        //if the button connected to pin 4 in port A is pressed then display the menu of the three modes 
        //and return from the function to wait for user input to enter any of th ethree modes
        
        if ((DIO_ReadPin(&GPIO_PORTA_DATA_R,4)) == 1) {
          
            LCD_command(0x01); // Clear LCD screen
            LCD_printString("Mode : 1.Calc");
            LCD_command(0xC0);
            LCD_printString("2.Timer 3.SW");
            
            return;
    } 
        
      }
       
      
}
  

}

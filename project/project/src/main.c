/***********************************************************************
 * 
 * Use Analog-to-digital conversion to read push buttons on LCD keypad
 * shield and display it on LCD screen.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2018 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions
#include <uart.h>           // Peter Fleury's UART library
#include <twi.h>            // I2C/TWI library for AVR-GCC
// values for joystick
uint16_t value_x =512;
uint16_t value_y=512;
uint16_t value_click;
uint8_t axisforjoystick = 0;
// values for rotary encoder
uint8_t rotaryclkk = 0;
uint8_t rotarydataa = 0;
uint8_t rotarypushh;
uint8_t rotaryclklaststate; 
uint16_t rotary;
// defines for rotary encoder
#define ROTARYSWITCH PD3
#define ROTARYDATA PD2
#define ROTARYCLK PC5

// defines for joystick
#define SWjoystick PC2

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and start ADC conversion every 100 ms.
 *           When AD conversion ends, send converted value to LCD screen.
 * Returns:  none
 **********************************************************************/
uint16_t PINcode= 1234;
uint16_t email=1204;
uint16_t phone=2345;
uint16_t job=2297;
uint16_t notebook=7264;    
int main(void)
{
    // Initialize display
    lcd_init(LCD_DISP_ON);
    uart_init(UART_BAUD_SELECT(9600, F_CPU));
    lcd_gotoxy(0, 0); lcd_puts("reminder ");
    lcd_gotoxy(0, 1); lcd_puts("password: ");
    twi_init();
    //lcd_gotoxy(8, 0); lcd_puts("a");  // Put ADC value in decimal
    //lcd_gotoxy(13,0); lcd_puts("b");  // Put ADC value in hexadecimal
    //lcd_gotoxy(8, 1); lcd_puts("c");  // Put button name here

    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX |= (1<<REFS0);
    ADMUX &= ~(1<<REFS1);
    // Select input channel ADC0 (voltage divider pin)
    ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0));
    // Enable ADC module
    ADCSRA |= (1 << ADEN);
    // Enable conversion complete interrupt
    ADCSRA |=(1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 33 ms and enable overflow interrupt
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();
    //GPIO pins for rotary
    GPIO_mode_input_pullup(&DDRB, ROTARYCLK);
    GPIO_mode_input_pullup(&DDRB, ROTARYDATA);
    GPIO_mode_input_pullup(&DDRB, ROTARYSWITCH);
    rotaryclklaststate = GPIO_read(&PORTB, ROTARYCLK);
    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}




/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 100 ms.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{   
    char string [4];// Start ADC conversion
    ADCSRA |= (1 << ADSC);
    // rotaryclkk = GPIO_read(&PORTB, ROTARYCLK);
    // rotarydataa = GPIO_read(&PORTB, ROTARYDATA);
    // rotarypushh = GPIO_read(&PORTB, ROTARYSWITCH);
    // if (rotaryclkk != rotaryclklaststate)
    //     {
    //         if (rotarydataa != rotaryclkk)
    //         {
    //             if (value_x>= 500 && value_x <=530 && value_y>= 500 && value_y <=530 )
    //                 {
    //                     PINcode =+1;
    //                     itoa(phone,string,10);
    //                     lcd_gotoxy(8,0);
    //                     lcd_puts("    ");
    //                     lcd_gotoxy(8,0);
    //                     lcd_puts(string);
    //                 }
    //             // else if (value_y >= 300 && value_y <= 600)
    //             //     {
    //             //         phone++;
    //             //     }
    //             // else if (value_x >= 500 && value_x <= 530 && value_y > 950)
    //             //     {
    //             //         job++;
    //             //     }
    //             // else if (value_x >= 500 && value_x <= 530 && value_y < 100)
    //             //     {
    //             //         email++;
    //             //     }
    //             // else if (value_x >= 300 && value_x <= 600)
    //             //     {
    //             //         notebook++;
    //             //     }
    //         }
    //         else {
    //             if (value_x>= 500 && value_x <=530 && value_y>= 500 && value_y <=530 )
    //                 {   
    //                     PINcode --;
    //                     itoa(phone,string,10);
    //                     lcd_gotoxy(8,0);
    //                     lcd_puts("    ");
    //                     lcd_gotoxy(8,0);
    //                     lcd_puts(string);
    //                 }
    //             // else if (value_y >= 300 && value_y <= 600)
    //             //     {
    //             //         phone--;
    //             //     }
    //             // else if (value_x >= 500 && value_x <= 530 && value_y > 950)
    //             //     {
    //             //         job--;
    //             //     }
    //             // else if (value_x >= 500 && value_x <= 530 && value_y < 100)
    //             //     {
    //             //         email--;
    //             //     }
    //             // else if (value_x >= 300 && value_x <= 600)
    //             //     {
    //             //         notebook=-1;
    //             //     }
    //         }
    //         rotaryclklaststate = rotaryclkk; 
    //     }
    // // switch (axisforjoystick)
    // // {
    // //     case(0):
    // //         ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2| 1<<MUX1| 1<<MUX0);
    // //         axisforjoystick=0;
    // //     case(1):
    // //         ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2| 1<<MUX1); ADMUX|= (1<<MUX0);
    // //         axisforjoystick=1;
    // // }  

}
void encoder()
{   static uint16_t PINcode= 1234;
    char string [4];// Start ADC conversion
    rotaryclkk = GPIO_read(&PORTB, ROTARYCLK);
    rotarydataa = GPIO_read(&PORTB, ROTARYDATA);
    rotarypushh = GPIO_read(&PORTB, ROTARYSWITCH);
    if (rotaryclkk != rotaryclklaststate)
        {
            if (rotarydataa != rotaryclkk)
            {
                if (value_x>= 500 && value_x <=530 && value_y>= 500 && value_y <=530 )
                    {   PINcode +=1;
                        lcd_gotoxy(11,1);
                        lcd_puts("      ");
                        lcd_gotoxy(11,1);
                        lcd_puts("PIN");
                        itoa(PINcode,string,10);
                        lcd_gotoxy(8,0);
                        lcd_puts("    ");
                        lcd_gotoxy(8,0);
                        lcd_puts(string);
            
                    }
                // else if (value_y >= 300 && value_y <= 600)
                //     {
                //         phone++;
                //     }
                // else if (value_x >= 500 && value_x <= 530 && value_y > 950)
                //     {
                //         job++;
                //     }
                // else if (value_x >= 500 && value_x <= 530 && value_y < 100)
                //     {
                //         email++;
                //     }
                // else if (value_x >= 300 && value_x <= 600)
                //     {
                //         notebook++;
                //     }
            }
            else {
                if (value_x>= 500 && value_x <=530 && value_y>= 500 && value_y <=530 )
                    {   
                        PINcode -=1;
                        lcd_gotoxy(11,1);
                        lcd_puts("      ");
                        lcd_gotoxy(11,1);
                        lcd_puts("PIN");
                        itoa(PINcode,string,10);
                        lcd_gotoxy(8,0);
                        lcd_puts("    ");
                        lcd_gotoxy(8,0);
                        lcd_puts(string);
                    }
                // else if (value_y >= 300 && value_y <= 600)
                //     {
                //         phone--;
                //     }
                // else if (value_x >= 500 && value_x <= 530 && value_y > 950)
                //     {
                //         job--;
                //     }
                // else if (value_x >= 500 && value_x <= 530 && value_y < 100)
                //     {
                //         email--;
                //     }
                // else if (value_x >= 300 && value_x <= 600)
                //     {
                //         notebook=-1;
                //     }
            }
            rotaryclklaststate = rotaryclkk; 
        }
    // switch (axisforjoystick)
    // {
    //     case(0):
    //         ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2| 1<<MUX1| 1<<MUX0);
    //         axisforjoystick=0;
    //     case(1):
    //         ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2| 1<<MUX1); ADMUX|= (1<<MUX0);
    //         axisforjoystick=1;
    // }  
}
/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/
ISR(ADC_vect)
{   char string [4];

    static uint8_t no_of_overflows = 0;
    if (axisforjoystick ==0)
        {
           ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2| 1<<MUX1| 1<<MUX0);
           value_x = ADC;
           axisforjoystick = 1;
        }
    if (axisforjoystick ==1)
        {
           ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2| 1<<MUX1); ADMUX|= (1<<MUX0);
           value_y = ADC;
        }
    if (no_of_overflows >= 3)
    {
        no_of_overflows = 0;
        no_of_overflows++;
        }
        
    // if (axisforjoystick ==0)
    // {
    //     value_x = ADC;}
    // if (axisforjoystick ==1)
    // {
    //     value_y = ADC;
       
    // }
        // Read converted value
        // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
        //value = ADC;
         // Convert "value" to "string" and display it


        // Button name
        lcd_gotoxy(8, 1);
        lcd_puts("      "); // Clear previous value
        lcd_gotoxy(8, 1);
        if (value_x>= 510 && value_x <=520 && value_y>= 500 && value_y <=520 ) // Middle
        {  lcd_clrscr; 
            lcd_gotoxy(11,1);
            lcd_puts("      ");
            lcd_gotoxy(11,1);
            lcd_puts("PIN");
            itoa(PINcode,string,10);
            lcd_gotoxy(8,0);
            lcd_puts("    ");
            lcd_gotoxy(8,0);
            lcd_puts(string);
            
        } 
        else if (value_y>= 510 && value_y <=520 && value_x>= 800 && value_x<=1024 ) // Up
        {
            lcd_clrscr;
            lcd_gotoxy(11,1);
            lcd_puts("      ");
            lcd_gotoxy(11,1);
            lcd_puts("phone");
            itoa(phone,string,10);
            lcd_gotoxy(8,0);
            lcd_puts("    ");
            lcd_gotoxy(8,0);
            lcd_puts(string);
        }
        else if (value_x >= 500 && value_x <= 530 && value_y > 900) // Up
        {
            lcd_clrscr;
            lcd_gotoxy(11,1);
            lcd_puts("      ");
            lcd_gotoxy(11,1);
            lcd_puts("job");
            itoa(job,string,10);
            lcd_gotoxy(8,0);
            lcd_puts("    ");
            lcd_gotoxy(8,0);
            lcd_puts(string);
        }
        else if (value_x >= 500 && value_x <= 530 && value_y < 100) // Down
        {
            lcd_clrscr;
            itoa(email,string,10);
            lcd_gotoxy(11,1);
            lcd_puts("      ");
            lcd_gotoxy(11,1);
            lcd_puts("email");
            lcd_gotoxy(8,0);
            lcd_puts("    ");
            lcd_gotoxy(8,0);
            lcd_puts(string);
        }
        else if (value_y >= 500 && value_y <= 530 && value_x < 490) // Right
        {
            lcd_clrscr;
            lcd_gotoxy(11,1);
            lcd_puts("      ");
            lcd_gotoxy(11,1);
            lcd_puts("ntbk");
            itoa(notebook,string,10);
            lcd_gotoxy(8,0);
            lcd_puts("    ");
            lcd_gotoxy(8,0);
            lcd_puts(string);
        }
        else
        {   lcd_clrscr;
            lcd_gotoxy(8,0);
            lcd_puts(" ");
      }
    }


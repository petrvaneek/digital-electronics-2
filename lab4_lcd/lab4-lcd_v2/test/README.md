# Lab 4: Petr Vaněk

### Stopwatch

1. Draw a flowchart for `TIMER2_OVF_vect` interrupt service routine which overflows every 16&nbsp;ms but it updates the stopwatch LCD screen approximately every 100&nbsp;ms (6 x 16&nbsp;ms = 100&nbsp;ms). Display tenths of a second, seconds, and minutes and let the stopwatch counts from `00:00.0` to `59:59.9` and then starts again. The image can be drawn on a computer or by hand. Use clear descriptions of the individual steps of the algorithms.

![image](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/7de069ee-da87-4346-b9e3-9100953782fb)



### Kitchen alarm
```c
/***********************************************************************
 * 
 * Stopwatch by Timer/Counter2 on the Liquid Crystal Display (LCD).
 *
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2017 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 * Components list:
 *   16x2 character LCD with parallel interface
 *     VSS  - GND (Power supply ground)
 *     VDD  - +5V (Positive power supply)
 *     Vo   - (Contrast)
 *     RS   - PB0 (Register Select: High for Data transfer, Low for Instruction transfer)
 *     RW   - GND (Read/Write signal: High for Read mode, Low for Write mode)
 *     E    - PB1 (Read/Write Enable: High for Read, falling edge writes data to LCD)
 *     D3:0 - NC (Data bits 3..0, Not Connected)
 *     D4   - PD4 (Data bit 4)
 *     D5   - PD5 (Data bit 5)
 *     D6   - PD6 (Data bit 6)
 *     D7   - PD7 (Data bit 7)
 *     A+K  - Back-light enabled/disabled by PB2
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD screen when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
#define N_CHARS 1
int main(void)
{
    // Initialize display
    //lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    // Put string(s) on LCD screen
    // lcd_gotoxy(6, 1);
    //lcd_puts("LCD Test");
    //lcd_putc('!');
    /*lcd_gotoxy(1,1);
    lcd_putc('b');
    lcd_gotoxy(11,0);
    lcd_putc('c');
    lcd_gotoxy(11,1);
    lcd_putc('a');*/
    /*uint8_t customChar[N_CHARS*8] = {
        0b00111,
        0b01110,
        0b11100,
        0b11000,
        0b11100,
        0b01110,
        0b00111,
        0b00011
    };*/
  uint8_t customChar [N_CHARS*48] = {
    // addr 0: .....
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000,
    // addr 1: |....
    0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000, 0b10000,
    // addr 2:
    0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000, 0b11000,
    //addr 3:
    0b11100, 0b11100, 0b11100, 0b11100, 0b11100, 0b11100,0b11100,0b11100,
    // addr 4:
    0b11110, 0b11110, 0b11110, 0b11110, 0b11110,0b11110,0b11110, 0b11110,
    // addr 5:
    0b11111, 0b11111, 0b11111, 0b11111, 0b11111,0b11111,0b11111, 0b11111


    };
    // Configuration of 8-bit Timer/Counter2 for Stopwatch update
    // Set the overflow prescaler to 16 ms and enable interrupt
    TIM2_overflow_16ms();
    TIM2_overflow_interrupt_enable();
    TIM1_overflow_262ms();
    TIM1_overflow_interrupt_enable();
        // Initialize display
    lcd_init(LCD_DISP_ON);
    
    lcd_command(1<<LCD_CGRAM);       // Set addressing to CGRAM (Character Generator RAM)
                                     // ie to individual lines of character patterns
    for (uint8_t i = 0; i < N_CHARS*48; i++)  // Copy new character patterns line by line to CGRAM
        lcd_data(customChar[i]);
    lcd_command(1<<LCD_DDRAM); 
    lcd_command(1<<LCD_DDRAM);       // Set addressing back to DDRAM (Display Data RAM)
                                     // ie to character codes
    // Display symbol with Character code 0
    //lcd_putc(0x00);
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Update the stopwatch on LCD screen every sixth overflow,
 *           ie approximately every 100 ms (6 x 16 ms = 100 ms).
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{   static uint32_t square = 0;
    static uint8_t symbol = 0;
    static uint8_t position = 0;
    static uint8_t no_of_overflows = 0;
    static uint8_t tenths = 0;  // Tenths of a second
    static uint8_t seconds = 0;
    static uint8_t minutes = 0;
    char string[2];             // String for converted numbers by itoa()
    no_of_overflows++;
    lcd_gotoxy(1+position,1);
    if (no_of_overflows ==0)
            {
                
                symbol = 0;
            }
    else if (no_of_overflows ==1)
    {
         symbol = 1;
    }
    else if (no_of_overflows ==2)
    {
         symbol = 2;
    }
    else if (no_of_overflows ==3)
    {
         symbol = 3;
    }
    else if (no_of_overflows ==4)
    {
         symbol = 4;
    }
    else if (no_of_overflows ==5)
    {
         symbol = 5;
    }
    
    lcd_putc(symbol);
    if (no_of_overflows >= 6)
    {   
        // Do this every 6 x 16 ms = 100 ms
        no_of_overflows = 0;
        tenths+=1;
        lcd_gotoxy(1+position, 1);
        //lcd_putc(symbol);
        position+=1;
        //symbol =0;

       /* if (tenths ==2)
            {
                
                symbol = 1;
            }
        else if (tenths ==4)
        {
            symbol = 2;
        }
        else if (tenths ==6)
        {
            symbol = 3;
        }
        else if (tenths ==8)
        {
            symbol = 4;
        }
        else if (tenths ==9)
        {
            symbol = 5;
        }
        else if (tenths ==0)
        {
            symbol = 0;
        }*/
        // Count tenth of seconds 0, 1, ..., 9, 0, 1, ...
        if (tenths > 9)
            {   symbol=0;
                tenths = 0;
                seconds += 1;
                square = seconds * seconds;
                itoa(square,string,10);
                lcd_gotoxy(11,0);
                lcd_puts(string);
                position = 0;
                lcd_gotoxy(1,1);
                lcd_putc(symbol);
                lcd_gotoxy(2,1);
                lcd_putc(symbol);
                lcd_gotoxy(3,1);
                lcd_putc(symbol);
                lcd_gotoxy(4,1);
                lcd_putc(symbol);
                lcd_gotoxy(5,1);
                lcd_putc(symbol);
                lcd_gotoxy(6,1);
                lcd_putc(symbol);
                lcd_gotoxy(7,1);
                lcd_putc(symbol);
                lcd_gotoxy(8,1);
                lcd_putc(symbol);
                lcd_gotoxy(9,1);
                lcd_putc(symbol);
                lcd_gotoxy(10,1);
                lcd_putc(symbol);
                lcd_gotoxy(11,1);
                lcd_putc(symbol);

            }
        if (seconds > 59)
            {
                seconds = 0;
                minutes += 1;
            }

        //itoa(tenths, string, 10);  // Convert decimal value to string
        // Display "00:00.tenths"
        //lcd_gotoxy(7, 0);
        //lcd_puts(string);
        itoa(minutes,string,10);
        lcd_gotoxy(1,0);
        lcd_puts(string);
        lcd_putc(':');
        itoa(seconds,string,10);
        //lcd_gotoxy(4, 0);
        lcd_puts(string);
        lcd_putc('.');
        itoa(tenths,string,10);
        //lcd_gotoxy(7, 0);
        lcd_puts(string);


    }
    //GPIO_mode_output(&DDRB,PB2);
    //GPIO_write_low(&PORTB,PB2);
    // Else do nothing and exit the ISR
}

ISR(TIMER1_OVF_vect)
{   lcd_gotoxy(11,1);
    uint8_t running_text[] = "   I like Digital electronics!\n";
    static uint8_t no_of_overflows = 0;
    static uint8_t text_position = 0;
    no_of_overflows+=1;
    if (no_of_overflows>=2)
        {
            static uint8_t text_position = 0;
            lcd_puts(running_text + text_position);
            size_t length =  strlen(running_text);
            text_position++;
            if (text_position >= length) {
                text_position = 0;
            }
        }    

}
```
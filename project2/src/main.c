/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Toggle two LEDs using the internal 8- and 16-bit 
 *           Timer/Counter.
 * Returns:  none
 **********************************************************************/
#define ANGLE1 124
#define ANGLE2 250
volatile uint16_t MOVE = 0;
volatile uint16_t pos1 = ANGLE1;
volatile uint16_t pos1 = ANGLE2;


uint8_t i = 0;
int main(void)
{   TIM0_overflow_16ms();
    TIM0_overflow_interrupt_enable();
    static counter = 0;
    DDRB = _BV(PB1);
    // set the comparator registers
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
    TCCR1B = _BV(CS11);
    OCR1A = 124;
    if (OCR1A ==124)
         {
             i = 0;

         }
     if (OCR1A ==250)
         {
            i = 1;
                     
        }
    if (i == 0)
     {
          for (OCR1A = 124;OCR1A<=250;OCR1A+=1);
            
                
     }        

    else if (i == 1)
     {
         for (OCR1A = 250;OCR1A >=124;OCR1A-=1);
                 
 
     }
    sei();
    while (1)
    
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}
ISR(TIMER0_OVF_vect)
{
    static uint8_t positon1 = 0;
    if (pos1 ==ANGLE)
}
# Lab 3: YOUR_FIRSTNAME LASTNAME

### Overflow times

1. Complete table with overflow times.


   | **Module** | **Number of bits** | **1** | **8** | **32** | **64** | **128** | **256** | **1024** |
   | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
   | Timer/Counter0 | 8  | 16u | 128u | -- | 1024u | -- | 4096u | 16384u |
   | Timer/Counter1 | 16 |  4096u   |   32768u   | -- | 262144u | -- | 1,048576 | 4,194304 |
   | Timer/Counter2 | 8  |   16u  |   128u   |  512u  | 1024u |  2048u  | 4096u | 16384u |

### Interrupts

2. In `timer.h` header file, define macros also for Timer/Counter2. Listing of part of the header file with settings for Timer/Counter2. Always use syntax highlighting, meaningful comments, and follow C guidelines:
#ifndef TIMER_H
 */
/** @brief Stop timer, prescaler 000 --> STOP */
#define TIM1_stop()           TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));
/** @brief Set overflow 4ms, prescaler 001 --> 1 */
#define TIM1_overflow_4ms()   TCCR1B &= ~((1<<CS12) | (1<<CS11)); TCCR1B |= (1<<CS10);
/** @brief Set overflow 33ms, prescaler 010 --> 8 */
#define TIM1_overflow_33ms()  TCCR1B &= ~((1<<CS12) | (1<<CS10)); TCCR1B |= (1<<CS11);
/** @brief Set overflow 262ms, prescaler 011 --> 64 */
#define TIM1_overflow_262ms() TCCR1B &= ~(1<<CS12); TCCR1B |= (1<<CS11) | (1<<CS10);
/** @brief Set overflow 1s, prescaler 100 --> 256 */
#define TIM1_overflow_1s()    TCCR1B &= ~((1<<CS11) | (1<<CS10)); TCCR1B |= (1<<CS12);
/** @brief Set overflow 4s, prescaler // 101 --> 1024 */
#define TIM1_overflow_4s()    TCCR1B &= ~(1<<CS11); TCCR1B |= (1<<CS12) | (1<<CS10);

/** @brief Enable overflow interrupt, 1 --> enable */
#define TIM1_overflow_interrupt_enable()  TIMSK1 |= (1<<TOIE1);
/** @brief Disable overflow interrupt, 0 --> disable */
#define TIM1_overflow_interrupt_disable() TIMSK1 &= ~(1<<TOIE1);


/**

 * @name  Definitions for 8-bit Timer/Counter0
 * @note  t_OVF = 1/F_CPU * prescaler * 2^n where n = 8, F_CPU = 16 MHz
 */
// WRITE YOUR CODE HERE
/** @brief Stop timer, prescaler 000 --> STOP */
#define TIM0_stop()           TCCR0B &= ~((1<<CS02)|(1<<CS01) | (1<<CS00));
/** @brief Set overflow 16us, prescaler 001 --> 1 */
#define TIM0_overflow_16us()   TCCR0B &= ~((1<<CS02)|(1<<CS01)); TCCR0B|= (1<<CS00);
/** @brief Set overflow 128us, prescaler 010 --> 8 */
#define TIM0_overflow_128us() TCCR0B &= ~((1<<CS02) | (1<<CS00)); TCCR0B |= (1<<CS01);
/** @brief Set overflow 1ms, prescaler 011 --> 64 */
#define TIM0_overflow_1ms() TCCR0B &= ~(1<<CS02); TCCR0B |= (1<<CS01) | (1<<CS00);
/** @brief Set overflow 4ms, prescaler 100 --> 256 */
#define TIM0_overflow_4ms() TCCR0B &= ~((1<<CS01) | (1<<CS00)); TCCR0B |= (1<<CS02);
/** @brief Set overflow 16ms, prescaler // 101 --> 1024 */
#define TIM0_overflow_16ms() TCCR0B &= ~(1<<CS01); TCCR0B |= (1<<CS02) | (1<<CS00);
/** @brief Enable overflow interrupt, 1 --> enable */
#define TIM0_overflow_interrupt_enable()  TIMSK0 |= (1<<TOIE0);
/** @brief Disable overflow interrupt, 0 --> disable */
#define TIM0_overflow_interrupt_disable() TIMSK0 &= ~(1<<TOIE0);

/**
 * @name  Definitions for 8-bit Timer/Counter2
 * @note  t_OVF = 1/F_CPU * prescaler * 2^n where n = 8, F_CPU = 16 MHz
 */
// WRITE YOUR CODE HERE
#define TIM2_stop()           TCCR2B &= ~((1<<CS22)|(1<<CS21) | (1<<CS20));
#define TIM2_overflow_16us()   TCCR2B &= ~((1<<CS22)|(1<<CS21)); TCCR2B|= (1<<CS20);
#define TIM2_overflow_128us() TCCR2B &= ~((1<<CS22)|(1<<CS21));TCCR2B|= (1<<CS20);
#define TIM2_overflow_512us() TCCR2B &= ~(1<<CS22);TCCR2B|=(1<<CS21) | (1<<CS20);
#define TIM2_overflow_1ms() TCCR2B &= ~((1<<CS21) | (1<<CS20)); TCCR2B |= (1<<CS22);
#define TIM2_overflow_2ms() TCCR2B &= ~(1<<CS21); TCCR2B |= (1<<CS22) | (1<<CS20);
#define TIM2_overflow_4ms()   {   TCCR2B |= (1<<CS22);\
                                TCCR2B |= (1<<CS21);\
                                TCCR2B &= ~(1<<CS20);\
}
#define TIM2_overflow_16ms() {   TCCR2B |= (1<<CS22);\
                                TCCR2B |= (1<<CS21);\
                                TCCR2B |= (1<<CS20);\
}
#define TIM2_overflow_interrupt_enable()  TIMSK2 |= (1<<TOIE2);
#define TIM2_overflow_interrupt_disable() TIMSK2 &= ~(1<<TOIE2);
int main(void)
{
    // Set pins where LEDs are connected as output
    GPIO_mode_output(&DDRB, LED_GREEN);

    // Configuration of 16-bit Timer/Counter1 for LED blinking
    // Set the overflow prescaler to 262 ms and enable interrupt
    TIM0_overflow_16ms();
    TIM0_overflow_interrupt_enable();

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
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Toggle on-board LED.
 **********************************************************************/
ISR(TIMER0_OVF_vect)
{   static uint8_t no_of_overflows = 0;

    no_of_overflows++;
    if (no_of_overflows >= 60)
    {
        // Do this every 6 x 16 ms = 100 ms
        no_of_overflows = 0;
        PORTB = PORTB ^ (1<<LED_GREEN);
        
    }
    TCNT0 = 50;
    // Else do nothing and exit the ISR
    
}

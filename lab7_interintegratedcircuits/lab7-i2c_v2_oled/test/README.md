# Lab 7: Petr Vaněk
### Arduino Uno pinout

1. In the picture of the Arduino Uno board, mark the pins that can be used for the following functions/operations:
   * PWM generators from Timer0, Timer1, Timer2
   * analog channels for ADC
   * UART pins
   * I2C pins
   * SPI pins
   * external interrupt pins INT0, INT1

![image](https://user-images.githubusercontent.com/99393183/201674306-e0da189c-07f3-44b7-958d-8f15318e5397.png)

### I2C communication

2. Draw a timing diagram of I2C signals when calling function `rtc_read_years()`. Let this function reads one byte-value from RTC DS3231 address `06h` (see RTC datasheet) in the range `00` to `99`. Specify when the SDA line is controlled by the Master device and when by the Slave device. Draw the whole request/receive process, from Start to Stop condition. The image can be drawn on a computer (by [WaveDrom](https://wavedrom.com/) for example) or by hand. Name all parts of timing diagram.

![image](https://user-images.githubusercontent.com/99393183/201685132-5100073a-deab-48e7-9fb2-d82183dc8d7c.png)

### Meteo station

Consider an application for temperature and humidity measurements. Use sensor DHT12, real time clock DS3231, LCD, and one LED. Every minute, the temperature, humidity, and time is requested from Slave devices and values are displayed on LCD screen. When the temperature is above the threshold, turn on the LED.

3. Draw a flowchart of `TIMER1_OVF_vect` (which overflows every 1&nbsp;sec) for such Meteo station. The image can be drawn on a computer or by hand. Use clear description of individual algorithm steps.

![image](https://user-images.githubusercontent.com/99393183/201694945-be4fc074-9a82-4064-a14f-e73df13f90ce.png
```c
/***********************************************************************
 * 
 * The I2C (TWI) bus scanner tests all addresses and detects devices
 * that are connected to the SDA and SCL signals.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2023 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Defines -----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000  // CPU frequency in Hz required for UART_BAUD_SELECT
#endif


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <twi.h>            // I2C/TWI library for AVR-GCC
#include <uart.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions


/* Global variables --------------------------------------------------*/


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Call function to test all I2C (TWI) combinations and send
 *           detected devices to UART.
 * Returns:  none
 * 
 * Some known devices:
 *     0x3c - OLED display
 *     0x57 - EEPROM
 *     0x5c - Temp+Humid
 *     0x68 - RTC
 *     0x68 - GY521
 *     0x76 - BME280
 *
 **********************************************************************/
/* Global variables --------------------------------------------------*/
// Declaration of "dht12" variable with structure "DHT_values_structure"
struct DHT_values_structure {
   uint8_t hum_int;
   uint8_t hum_dec;
   uint8_t temp_int;
   uint8_t temp_dec;
   uint8_t checksum;
} dht12;
struct RTC_values_structure {
    uint8_t secs;
    uint8_t mins;
    uint8_t hours;
} rtc;
// Flag for printing new data from sensor
volatile uint8_t new_sensor_data = 0;


// Slave and internal addresses of temperature/humidity sensor DHT12
#define SENSOR_ADR 0x5c
#define SENSOR_HUM_MEM 0
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4
#define RTC_ADR 0x68
#define RTC_SEC_MEM 0
#define RTC_MIN_MEM 1
#define RTC_HOUR_MEM 2


int main(void)
{
    char string[2];  // String for converting numbers by itoa()

    // TWI
    twi_init();

    // UART
    uart_init(UART_BAUD_SELECT(115200, F_CPU));

    sei();  // Needed for UART
    uart_puts("Scanning I2C... ");
    for (uint8_t sla = 8; sla < 120; sla++) {
        if (twi_test_address(sla) == 0) {  // ACK from Slave
            uart_puts("\r\n");
            itoa(sla, string, 16);
            uart_puts(string);
        }
    }
    // Test if sensor is ready
    if (twi_test_address(SENSOR_ADR) == 0)
        uart_puts("I2C sensor detected\r\n");
    else {
        uart_puts("[ERROR] I2C device not detected\r\n");
        while (1);
    }

    // Timer1
    TIM1_overflow_1s();
    TIM1_overflow_interrupt_enable();

    sei();

    // Infinite loop
    while (1) {
        if (new_sensor_data == 1) {
            itoa(dht12.temp_int, string, 10);
            uart_puts(string);
            uart_puts(".");
            itoa(dht12.temp_dec, string, 10);
            uart_puts(string);
            uart_puts(" °C\r\n");
            itoa(dht12.hum_int,string,10);
            uart_puts(string);
            uart_puts(".");
            itoa(dht12.hum_dec,string,10);
            uart_puts(string);
            uart_puts(" percent\r\n");
            itoa(dht12.checksum,string,10);
            uart_puts(string);
            uart_puts(" total\r\n");
            itoa(rtc.hours,string,10);
            uart_puts(string);
            uart_puts(":");
            itoa(rtc.mins,string,10);
            uart_puts(string);
            uart_puts(":");
            itoa(rtc.secs,string,10);
            uart_puts(string);
            uart_puts("time\r\n");
            

            // Do not print it again and wait for the new data
            new_sensor_data = 0;
        }
    }

    // Will never reach this
    return 0;
}
void rtc_read_seconds()
{
    twi_start();
    if (twi_write((RTC_ADR << 0) | TWI_WRITE) == 0) {
        // Set internal memory location
        twi_write(RTC_SEC_MEM);
        twi_stop();
        // Read data from internal memory
        twi_start();
        twi_write((RTC_ADR<<1) | TWI_READ);
        rtc.secs = twi_read(TWI_NACK);
        new_sensor_data = 1;
    }
    twi_stop();
}


void rtc_read_minutes()
{
    twi_start();
    if (twi_write((RTC_ADR ) | TWI_WRITE) == 0) {
        // Set internal memory location
        twi_write(RTC_MIN_MEM);
        twi_stop();
        // Read data from internal memory
        twi_start();
        twi_write((RTC_ADR<<1) | TWI_READ);
        rtc.mins = twi_read(TWI_NACK);
        twi_stop();
        new_sensor_data = 1;
    }
    twi_stop();
}

void rtc_read_hours()
{
    twi_start();
    if (twi_write((RTC_ADR ) | TWI_WRITE) == 0) {
        // Set internal memory location
        twi_write(RTC_HOUR_MEM);
        twi_stop();
        // Read data from internal memory
        twi_start();
        twi_write((RTC_ADR<<1) | TWI_READ);
        rtc.hours = twi_read(TWI_NACK);
        twi_stop();
        new_sensor_data = 1;
    }
    twi_stop();
}

/* Interrupt service routines ----------------------------------------*/
ISR(TIMER1_OVF_vect)
{
    // Test ACK from sensor
    twi_start();
    if (twi_write((SENSOR_ADR<<1) | TWI_WRITE) == 0) {
        // Set internal memory location
        twi_write(SENSOR_TEMP_MEM);
        twi_stop();
        // Read data from internal memory
        twi_start();
        twi_write((SENSOR_ADR<<1) | TWI_READ);
        dht12.temp_int = twi_read(TWI_ACK);
        dht12.temp_dec = twi_read(TWI_ACK);
        dht12.hum_int = twi_read(TWI_ACK);
        dht12.hum_dec = twi_read(TWI_ACK);
        dht12.checksum = twi_read(TWI_NACK);
        new_sensor_data = 1;
        
    }
    twi_stop();
    rtc_read_seconds();
    rtc_read_minutes();
    rtc_read_minutes();
}

```

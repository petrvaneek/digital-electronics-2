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
#include <oled.h>


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
// GPS module


int main(void)
{   
    char string[2];  // String for converting numbers by itoa()
    
    // TWI
    twi_init();

    // UART
    uart_init(UART_BAUD_SELECT(9600, F_CPU));

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
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    sei();
        // Iterate through each line
   
    // Infinite loop
    while (1) {
        if (new_sensor_data == 1) {
                // Zobrazí jednotlivé hodnoty na displeji
        
            itoa(dht12.temp_int, string, 10);
            oled_gotoxy(0, 2);
            oled_puts(string);
            oled_puts(".");
            uart_puts(string);
            uart_puts(".");
            itoa(dht12.temp_dec, string, 10);
            oled_puts(string);
            uart_puts(string);
            uart_puts(" °C\r\n");
            oled_puts(" °C\r\n");
            itoa(dht12.hum_int,string,10);
            oled_puts(string);
            oled_puts(".");
            uart_puts(string);
            uart_puts(".");
            itoa(dht12.hum_dec,string,10);
            oled_puts(string);
            oled_puts(" percent\r\n");
            uart_puts(string);
            uart_puts(" percent\r\n");
            itoa(dht12.checksum,string,10);
            oled_puts(string);
            oled_puts(" total\r\n");
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
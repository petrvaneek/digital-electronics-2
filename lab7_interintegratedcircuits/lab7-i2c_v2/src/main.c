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
#define GPS_RX_PIN 0  // Pin 0 na Arduinu Uno
#define GPS_TX_PIN 1  // Pin 1 na Arduinu Uno

char data[80];
char data[] = "$GPRMC, 123545.00, A, 4913.58989, N, 01634.42942, E, 0.322, 21.61, 161123, A53\r\n$GPVTG 21.61 T A 0.322 0.597 K A01 $GPGGA 093533.00 4913.58989 N 01634.42942 E 1 03 4.64 261.4 M 42.4 M   56\r\n$GPGSA A 2 04 09 16           4.74 4.64 1.0009\r\n$GPGSV 2 1 06 04 21 082 36 07       35 09 55 078 40 13 00 263 42\r\n$GPGSV 2 2 06 16 16 041 36 20 47 2987D\r\n$GPGLL 4913.58989 N 01634.42942 E 093533.00 A A*65";
char formattedTime[9];
char connectionState;
char latitude[14];
char longitude[14];
char latitudeIndicator, longitudeIndicator;
char formattedDate[9];
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
    char *line = strtok(data, "\r\n");
    while (line != NULL) {
        // Check if the line starts with "$GPRMC"
        if (isGPRMC(line)) {
            // Extract and process GPRMC data

            // Extracting and formatting information using functions
            extractAndFormatTime(line, formattedTime);
            connectionState = extractConnectionState(line);
            extractLatitude(line, latitude);
            latitudeIndicator = extractLatitudeIndicator(line);
            extractLongitude(line, longitude);
            longitudeIndicator = extractLongitudeIndicator(line);
            extractAndFormatDate(line, formattedDate);

            // Displaying the extracted and formatted information
            printf("Time: %s\n", formattedTime);
            printf("Connection State: %c\n", connectionState);
            printf("Latitude: %s %c\n", latitude, latitudeIndicator);
            printf("Longitude: %s %c\n", longitude, longitudeIndicator);
            printf("Date: %s\n", formattedDate);

            // Concatenating the extracted and formatted information into a single line string
            char resultString[200];

            sprintf(resultString, "Time: %s, Date: %s, Connection State: %c, Latitude: %s %c, Longitude: %s %c",
                    formattedTime, formattedDate, connectionState, latitude, latitudeIndicator, longitude, longitudeIndicator);

            // Displaying the concatenated string
            printf("%s\n", resultString);
        }

        // Move to the next line
        line = strtok(NULL, "\r\n");
    }
    return 0;
    // Infinite loop
    while (1) {
        if (new_sensor_data == 1) {
                // Zobrazí jednotlivé hodnoty na displeji
        oled_gotoxy(10, 0);
        oled_puts("Time: ");
        oled_puts(formattedTime);

        oled_gotoxy(10, 1);
        oled_puts("Connection State: ");
        oled_putc(connectionState);

        oled_gotoxy(10, 2);
        oled_puts("Latitude: ");
        oled_puts(latitude);
        oled_putc(' ');
        oled_putc(latitudeIndicator);

        oled_gotoxy(10, 3);
        oled_puts("Longitude: ");
        oled_puts(longitude);
        oled_putc(' ');
        oled_putc(longitudeIndicator);

        oled_gotoxy(10, 4);
        oled_puts("Date: ");
        oled_puts(formattedDate);
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

void gps_module()
{   int i = 0;
    unsigned int receivedData;

    // Read data from UART into buffer
    receivedData = uart_getc();
    while (receivedData == UART_NO_DATA && i < sizeof(data) - 1) {
        char c = uart_getc();
        data[i++] = c;
    }

    // Write data from buffer to RTC memory
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_WRITE);
    twi_write(0x00);  // Specify the starting memory address in the RTC

    for (int j = 0; j < i; j++) {
        twi_write(data[j]);
    }
    twi_stop();
    // Read data from RTC memory back into buffer
    twi_start();
    twi_write((RTC_ADR << 1) | TWI_READ);

    for (int j = 0; j < i; j++) {
        data[j] = twi_read(j < i - 1 ? TWI_ACK : TWI_NACK);
        uart_putc(data[j]);
    }
    twi_stop();
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

// Function to extract time from the GPRMC sentence and format it as "XX:XX:XX"
void extractAndFormatTime(char *data, char *formattedTime) {
    char time[6];
    strncpy(time, data + 8, 6);
    sprintf(formattedTime, "%c%c:%c%c:%c%c", time[0], time[1], time[2], time[3], time[4], time[5]);
}

// Function to extract connection state from the GPRMC sentence
char extractConnectionState(char *data) {
    return data[19];
}

// Function to extract latitude from the GPRMC sentence
void extractLatitude(char *data, char *latitude) {
    strncpy(latitude, data + 22, 10);
}

// Function to extract N/S indicator from the GPRMC sentence
char extractLatitudeIndicator(char *data) {
    return data[34];
}

// Function to extract longitude from the GPRMC sentence
void extractLongitude(char *data, char *longitude) {
    strncpy(longitude, data + 37, 11);
}

// Function to extract E/W indicator from the GPRMC sentence
char extractLongitudeIndicator(char *data) {
    return data[50];
}

// Function to extract date from the GPRMC sentence and format it as "XX.XX.XX"
void extractAndFormatDate(char *data, char *formattedDate) {
    char date[7];
    strncpy(date, data + 67, 6);
    date[6] = '\0';
    sprintf(formattedDate, "%c%c.%c%c.%c%c", date[0], date[1], date[2], date[3], date[4], date[5]);
}

// Function to check if the line starts with "$GPRMC"
int isGPRMC(char *line) {
    return (strstr(line, "$GPRMC") != NULL);
}
void savedata()
    {
        saveDataToFile("output.txt", formattedTime, connectionState, latitude, latitudeIndicator, longitude, longitudeIndicator, formattedDate);
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
    gps_module();
    savedata();
}
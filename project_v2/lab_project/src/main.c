#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <oled.h>
#include <twi.h> 
#include "timer.h"  
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC


#define F_CPU 16000000UL  
#define BAUD_RATE 9600
// Define the size of the buffer used to store received data
#define BUFFER_SIZE 128  
// Declare structure of temperature sensor values
struct DHT_values_structure {
   uint8_t hum_int;
   
   uint8_t hum_dec;
   uint8_t temp_int;
   uint8_t temp_dec;
   uint8_t checksum;
} dht12;
// Slave and internal addresses of temperature/humidity sensor DHT12
#define SENSOR_ADR 0x5c
#define SENSOR_HUM_MEM 0
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4
// Flag for printing new data from sensor
volatile uint8_t new_sensor_data = 0;
// Declare an array to store received data from UART
char received_data[BUFFER_SIZE];
// Declare a volatile variable to keep track of the index in the received_data buffer
volatile uint8_t received_index = 0;
// Define a constant character array representing the prefix of a GPRMC SENTENCE
char gprmc_prefix[] = "$GPRMC";
// Function to initialize UART communication
void uart_init() {
    // Set the UART baud rate
    UBRR0H = (uint8_t)(F_CPU / (16UL * BAUD_RATE) - 1) >> 8;
    UBRR0L = (uint8_t)(F_CPU / (16UL * BAUD_RATE) - 1);

    // Enable receiver and transmitter of UART
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Function to receive a single character from UART
char uart_receive() {
    // Wait until data is available in the receive buffer
    while (!(UCSR0A & (1 << RXC0)));

    // Return received data
    return UDR0;
}

// Function to transmit a single character through UART
void uart_transmit(char data) {
    // Wait until the transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Send data
    UDR0 = data;
}

// Function to transmit a string of characters through UART
void uart_transmit_string(const char *str) {
    // Transmit each character in the string until the null terminator is reached
    for (size_t i = 0; i < strlen(str); i++) {
        uart_transmit(str[i]);
    }
}

// Function to extract time from the GPRMC sentence and format it as "XX:XX:XX"
void extractAndFormatTime(char *data, char *formattedTime) {
    // Define array size of time string + Null-terminate
    char time[7];
    // Extract the time part from the input data starting from the 7th character taking 6 characters
    strncpy(time, data + 7, 6);
    // Null-terminate the extracted time string to make it a valid C string
    time[6] = '\0';  
    // Format the time and store it in the formattedTime string
    sprintf(formattedTime, "%c%c:%c%c:%c%c", time[0], time[1], time[2], time[3], time[4], time[5]);
}

// Function to extract date from the GPRMC sentence and format it as "XX.XX.XX"
void extractAndFormatDate(char *data, char *formattedDate) {
    // Define array size of date string + Null-terminate
    char date[7];
    // Calculate the starting index for the date by subtracting 6 characters from the end of the string
    int startIndex = strlen(data) - 15;  // Adjusted for the date length
    // Extract the date part from the input data starting from the calculated index taking 6 characters
    strncpy(date, data + startIndex, 6);
    // Null-terminate the extracted date string to make it a valid C string
    date[6] = '\0';
    // Format the date and store it in the formattedDate string
    sprintf(formattedDate, "%c%c.%c%c.%c%c", date[0], date[1], date[2], date[3], date[4], date[5]);
}

// Function to extract latitude from the GPRMC sentence
void extractLatitude(char *data, char *latitude) {
    // Extract the time part from the input data starting from the 19th character taking 10 characters
    strncpy(latitude, data + 19, 10);  
    // Null-terminate the extracted time string to make it a valid C string
    latitude[10] = '\0';  
}

// Function to extract latitude direction N/S from the GPRMC sentence
char extractLatitudeDir(char *data) {
    // Extract the latitude direction from the input data from the 30th character
    return data[30];
}
// Function to extract longitude from the GPRMC sentence
void extractLongitude(char *data, char *longitude) {
    // Extract the time part from the input data starting from the 19th character taking 10 characters
    strncpy(longitude, data + 32, 11);
    // Null-terminate the extracted time string to make it a valid C string
    longitude[11] = '\0';
}

// Function to extract longitude direction E/W from the GPRMC sentence
char extractLongitudeDir(char *data) {
    // Extract the latitude direction from the input data from the 44th character
    return data[44];
}
int main(void) {
    uart_init();
    char string[2];  // String for converting numbers by itoa()

    // TWI

     // Timer1
    TIM1_OVF_1SEC
    TIM1_OVF_ENABLE

    sei();
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    oled_gotoxy(0, 0);
    oled_puts("Date:,");
    oled_gotoxy(0, 1);
    oled_puts("Time:,");
    oled_gotoxy(0, 2);  
    oled_puts("Temp[°C]:");
    oled_gotoxy(0, 3);
    oled_puts("Hum [\%]:");
    oled_gotoxy(0,4);
    oled_puts("Lat:,");
    oled_gotoxy(0,5);
    oled_puts("Lon:,");
    while (1) {
        // Receive data from the RX pin
        char received_char = uart_receive();
        
        // Save the received character into the buffer
        received_data[received_index] = received_char;
        // Increment the buffer index (with overflow protection)
        received_index = (received_index + 1) % BUFFER_SIZE;
        if (new_sensor_data == 1) {
            
            itoa(dht12.temp_int, string, 10);
            oled_gotoxy(10, 2);
            oled_puts(string);
            oled_puts(".");
            uart_transmit_string("Temperature:,");
            uart_transmit_string(dht12.temp_int);
            uart_transmit_string(".");
            itoa(dht12.temp_dec, string, 10);
            oled_puts(string);
            uart_transmit_string(dht12.temp_dec);
            uart_transmit_string(",");
            itoa(dht12.hum_int,string,10);
            oled_gotoxy(10, 3);
            oled_puts(string);
            oled_puts(".");
            uart_transmit_string("Humidity:,");
            uart_transmit_string(dht12.hum_int);
            uart_transmit_string(".");
            itoa(dht12.hum_dec,string,10);
            oled_puts(string);
            uart_transmit_string(dht12.hum_dec);
            uart_transmit_string(",");
            itoa(dht12.checksum,string,10);
            oled_display();


            // Otevření souboru nebo vytvoření, pokud neexistuje
            //FILE *file = fopen("data.txt","a");
            // Do not print it again and wait for the new data
            new_sensor_data = 0;
        }
        // Check for a newline character, indicating the end of a message
        if (received_char == '\n') {
            // Null-terminate the string
            received_data[received_index] = '\0';
                twi_start();
        
            // Check if the message starts with "$GPRMC"
            if (strncmp(received_data, gprmc_prefix, strlen(gprmc_prefix)) == 0) {
                // Extract and format the time, date, latitude, and longitude data
                char formattedTime[9];
                char formattedDate[9];
                char latitude[11];
                char longitude[13];
                char latitudeDir, longitudeDir;

                extractAndFormatTime(received_data, formattedTime);
                extractAndFormatDate(received_data, formattedDate);
                extractLatitude(received_data, latitude);
                extractLongitude(received_data, longitude);
                latitudeDir = extractLatitudeDir(received_data);
                longitudeDir = extractLongitudeDir(received_data);

                // Transmit the formatted data
                uart_transmit_string("Time:,");
                uart_transmit_string(formattedTime);
                uart_transmit_string(",");
                oled_gotoxy(7, 1);
                oled_puts(formattedTime);
                oled_puts(",");

                uart_transmit_string("Date:,");
                uart_transmit_string(formattedDate);
                uart_transmit_string(",");
                oled_gotoxy(7, 0);
                oled_puts(formattedDate);
                oled_puts(",");

                uart_transmit_string("Latitude:,");
                uart_transmit_string(latitude);
                uart_transmit_string(" ");
                uart_transmit(latitudeDir);
                uart_transmit_string(",");
                oled_gotoxy(6,4);
                oled_puts(latitude);
                oled_putc(latitudeDir);
                oled_puts(",");


                uart_transmit_string("Longitude:,");
                uart_transmit_string(longitude);
                uart_transmit_string(" ");
                uart_transmit(longitudeDir);
                oled_gotoxy(6,5);
                oled_puts(longitude);
                oled_puts(" ");
                oled_putc(longitudeDir);

                itoa(dht12.temp_int, string, 10);
                uart_transmit_string(",Temperature:,");
                uart_transmit_string(string);
                uart_transmit_string(".");
                itoa(dht12.temp_dec, string, 10);
                uart_transmit_string(string);
                uart_transmit_string(",°C");
                itoa(dht12.hum_int, string, 10);
                uart_transmit_string(",Humidity:, ");
                uart_transmit_string(string);
                uart_transmit_string(".");
                itoa(dht12.hum_dec, string, 10);
                uart_transmit_string(string);
                uart_transmit_string(",%,");




                uart_transmit_string("\r");
                uart_transmit_string("\n");
                oled_display();
                
            }
            // Reset the buffer index for the next message
            received_index = 0;
        }
    }
    return 0;
}

ISR(TIMER1_OVF_vect)
{
    static uint8_t n_ovfs = 0;

    n_ovfs++;
    // Read the data every 5 secs
    if (n_ovfs >= 5) {
        n_ovfs = 0;

        // Test ACK from Temp/Humid sensor
        twi_start();
        if (twi_write((SENSOR_ADR<<1) | TWI_WRITE) == 0) {
            // Set internal memory location
            twi_write(SENSOR_HUM_MEM);
            twi_stop();
            // Read data from internal memory
            twi_start();
            twi_write((SENSOR_ADR<<1) | TWI_READ);
            dht12.hum_int = twi_read(TWI_ACK);
            dht12.hum_dec = twi_read(TWI_ACK);
            dht12.temp_int = twi_read(TWI_ACK);
            dht12.temp_dec = twi_read(TWI_NACK);

            new_sensor_data = 1;
        }
        twi_stop();
    }
}
/*ISR(TIMER1_OVF_vect)
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
}*/


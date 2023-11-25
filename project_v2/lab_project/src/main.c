#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
//#include <uart.h>
#include <stdio.h>

#define BUFFER_SIZE 512

char sent_data[] = "$GPRMC, 123545.00, A, 4913.58989, N, 01634.42942, E, 0.322, 21.61, 161123, A53\r\n$GPVTG 21.61 T A 0.322 0.597 K A01 $GPGGA 093533.00 4913.58989 N 01634.42942 E 1 03 4.64 261.4 M 42.4 M   56\r\n$GPGSA A 2 04 09 16           4.74 4.64 1.0009\r\n$GPGSV 2 1 06 04 21 082 36 07       35 09 55 078 40 13 00 263 42\r\n$GPGSV 2 2 06 16 16 041 36 20 47 2987D\r\n$GPGLL 4913.58989 N 01634.42942 E 093533.00 A A*65";
char received_data[BUFFER_SIZE];
volatile uint8_t new_sensor_data = 0;
char Data;
/*void uart_send_string(const char *str) {
    while (*str != '\0') {
        uart_putc(*str);
        str++;

        Data = str++;
    }
    uart_putc('\n'); // Přidejte nový řádek na konec řetězce
    _delay_ms(10); // Přidejte zpoždění pro zajištění času na přenos dat

}*/
#define F_CPU 16000000UL  // Assuming a 16MHz clock frequency
#define BAUD_RATE 9600

void uart_init() {
    // Set baud rate
    UBRR0H = (uint8_t)(F_CPU / (16UL * BAUD_RATE) - 1) >> 8;
    UBRR0L = (uint8_t)(F_CPU / (16UL * BAUD_RATE) - 1);

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

char uart_receive() {
    // Wait until data is available
    while (!(UCSR0A & (1 << RXC0)));

    // Return received data
    return UDR0;
}

void uart_transmit(char data) {
    // Wait until the transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Send data
    UDR0 = data;
}

int main(void) {
    //uart_init(UART_BAUD_SELECT(9600, F_CPU));

    TIM1_overflow_1s();
    TIM1_overflow_interrupt_enable();

    sei();

    while (1) {
        if (new_sensor_data == 1) {
            // Poslat řetězec "david" z poslané zprávy přes UART
            //uart_send_string(sent_data);
            uart_transmit(sent_data);
            // Přijmout data ze sériového monitoru (poslaná zpráva)
            _delay_ms(100); // Přidejte zpoždění pro zajištění času na přenos dat

            // Poslat přijatý řetězec přes UART s použitím uart_puts_p
            //uart_send_string(uart_getc());
            // Receive data from the RX pin

        // Optionally, you can also view the data on the serial monitor
        // by printing the received character to the serial port.

            // Poslat "finish" přes UART
            //uart_puts("finish");

            new_sensor_data = 0;
        }
        char received_char = uart_receive();

        // Transmit the received character back to the computer
        uart_transmit(received_char);

    }

    return 0;
}

ISR(TIMER1_OVF_vect) {
    new_sensor_data = 1;
}

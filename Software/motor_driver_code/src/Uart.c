#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

#include "Uart.h"

/*
 *	Function: 		void USART0_init(uint32_t baud)
 *  Description: 	init USART0 with a specific baudrate
 */
void USART0_init(uint32_t baud)
{
	uint32_t temp_baud = 0;

	temp_baud = (F_CPU / (8 * baud)) - 1;

	//Set baud rate
	UBRR0H = (unsigned char) (temp_baud >> 8);
	UBRR0L = (unsigned char) temp_baud;

	UCSR0A |= (1 << U2X0);

	UCSR0B |= (1 << RXEN0);							// Disable RX

	DDRE |= (1 << PE1);								// Set TX as output
	DDRE &= ~(1 << PE0);							// Set RX as input

	UCSR0B |= (1 << TXEN0);							//Enable TX
	UCSR0B &= ~(1 << UCSZ02);						//Data size

	// async, no parity, 1 stop bit, 8 bit data size
	UCSR0C &= ~(1 << 7);
	UCSR0C &= ~(1 << UMSEL0);
	UCSR0C &= ~(1 << UPM01) ;
	UCSR0C &= ~(1 << UPM00);
	UCSR0C &= ~(1 << USBS0);
	UCSR0C &= ~(1 << USBS0);
	UCSR0C |= (1 << UCSZ01);
	UCSR0C |= (1 << UCSZ00);

} // end of USART0_init()

/*
 *	Function: 		void USART0_transmit(unsigned char data)
 *  Description: 	transmit unsigned char through USART0
 */
void USART0_transmit(unsigned char data)
{
	while ( ! ( UCSR0A & (1<<UDRE0))); 				// Wait for empty transmit buffer

	UDR0 = data; 									// Put data into buffer, sends the data

} // end of USART0_transmit()

/*
 *	Function: 		unsigned char USART0_receive(void)
 *  Description: 	receive data through USART0
 */
unsigned char USART0_receive(void)
{
	while ( ! (UCSR0A & (1<<RXC0)));				// Wait for data to be received

	return UDR0;									// return it
} // end of USART0_receive()

/*
 *	Function: 		void USART0_string(char *s)
 *  Description: 	transmit a string through USART0
 */
void USART0_string(char *s)
{
   	while (*s != 0x00)
   	{
		USART0_transmit(*s);
      	s++;
   	}
} // end of USART0_string()

/*
 * 	Function: 		void debug_printf(char *s)
 *  Description: 	print if DEBUG is defined
 */
void debug_printf(char *s) {
	#ifdef DEBUG
		USART0_string(*s);
	#endif
} // end of debug_printf()

/*
 * 	Function: 		void debug_printf_num(int num)
 *  Description: 	print if DEBUG is defined
 */
void debug_printf_num(int num) {
	#ifdef DEBUG
		char converted[30];
		itoa(num, msg, 10);
		USART0_string(converted);
	#endif
} // end of debug_printf()
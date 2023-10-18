#define F_CPU 16000000UL			/* Define frequency here its 8MHz */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#define USART_BAUD 9600UL
#define BAUD_PRESCALE (((F_CPU / (USART_BAUD * 16UL))) - 1)
#ifdef UDR0
#define UBRRH UBRR0H
#define UBRRL UBRR0L
#define UDR UDR0
#define UCSRA UCSR0A
#define UDRE UDRE0
#define RXC RXC0
#define UCSRB UCSR0B
#define RXEN RXEN0
#define TXEN TXEN0
#define RXCIE RXCIE0
#define UCSRC UCSR0C
#define URSEL 
#define UCSZ0 UCSZ00
#define UCSZ1 UCSZ01
#define UCSRC_SELECT 0
#else
#define UCSRC_SELECT (1 << URSEL)
#endif
#define USE_SLEEP 1

int flag = 0;
int encoder_count = 0;
int high_count=0;
char countstr[1];

void setup() {
	DDRD &= ~(1 << PD5); //input pin 5
 	PORTD |= (1 << PD5);
 	DDRB |= _BV(DDB5); //output led 13
	DDRB |= (1 << PB0);
        PORTB &= ~(1 << PB0); //output pin 8 trigger_enable
	DDRB |= (1 << PB2);
        PORTB &= ~(1 << PB2); //output pin 10 high_count
}
void UART_init(long USART_BAUDRATE)
{
	UCSRB |= (1 << RXEN) | (1 << TXEN);/* Turn on transmission and reception */
	UCSRC |= UCSRC_SELECT | (1 << UCSZ0) | (1 << UCSZ1);/* Use 8-bit character sizes */
	UBRRL = BAUD_PRESCALE;		/* Load lower 8-bits of the baud rate value */
	UBRRH = (BAUD_PRESCALE >> 8);	/* Load upper 8-bits*/
}

unsigned char UART_RxChar()
{
	while ((UCSRA & (1 << RXC)) == 0);/* Wait till data is received */
	return(UDR);			/* Return the byte*/
}

void UART_TxChar(char ch)
{
	while (! (UCSRA & (1<<UDRE)));	/* Wait for empty transmit buffer*/
	UDR = ch ;
}

void UART_SendString(char *str)
{
	unsigned char j=0;
	
	while (str[j]!=0)		/* Send string till null */
	{
		UART_TxChar(str[j]);	
		j++;
	}
}

void integertostr(uint8_t num, char *str){
	itoa(num,str,10);
	}

void encoder_st() {
	uint8_t array_a[5]={0};
	uint8_t currStateOfPin = 0;
	uint8_t nextStateOfPin = 0;
	for (; ; ) {
 		uint8_t curVal = (PIND & (1 << PD5))?1:0;
		//for (uint8_t j =0; j < 3; j++) {
		//     array_a[j] = array_a[j+1];
		//}
		array_a[0] = array_a[1];
		array_a[1] = array_a[2];
		array_a[2] = array_a[3];
		array_a[3] = array_a[4];
		array_a[4] = curVal;
		/*integertostr(array_a[4], countstr);
		UART_SendString(countstr);
		integertostr(array_a[3], countstr);
		UART_SendString(countstr);
		integertostr(array_a[2], countstr);
		UART_SendString(countstr);
		integertostr(array_a[1], countstr);
		UART_SendString(countstr);
		integertostr(array_a[0], countstr);
		UART_SendString(countstr);
*/
		if (currStateOfPin == 0) {
                     nextStateOfPin = array_a[4] & array_a[3]  &  array_a[2]  &  array_a[1]  &  array_a[0];
		}
		if (currStateOfPin == 1) {
                     nextStateOfPin = array_a[4] |  array_a[3]  |  array_a[2]  |  array_a[1]  |  array_a[0];
		}
		if (currStateOfPin == 0 & nextStateOfPin == 1) {
		    // rising edge
		    encoder_count = encoder_count + 1;
			if(encoder_count>=50 && encoder_count<60)			  
			{
				PORTB |= (1 << PB0); // trigger enable pin
			}
		
			else
			{
				PORTB &= ~(1 << PB0); // trigger disable pin
			}
		}
		currStateOfPin = nextStateOfPin;
		if(encoder_count==100)
		{
			encoder_count=0;
			PORTB &= ~(1 << PB0); // trigger disable pin
				
		}

	}
}

int main(void) {
	setup();
	char c;
	UART_init(USART_BAUD);
	//encoder_st();
	while(1){
		encoder_st();
	}
	return 0;
}

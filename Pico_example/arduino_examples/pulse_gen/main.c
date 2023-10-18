#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	DDRB |= (1 << PB3); //pin 11 output
	PORTB = (1 << PB3);
		while (1) {
			PORTB=~PORTB;
			_delay_us(200);
			//PORTB &= ~(1 << PB3);
			//_delay_ms(500);
		}
	return 0;
}

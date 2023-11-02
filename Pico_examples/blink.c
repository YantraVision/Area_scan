#include "pico/stdlib.h"

int main() {
	while(1){
		gpio_init(16);
    		gpio_set_dir(16, GPIO_OUT);
   		gpio_put(16,1);
   		sleep_us(100);
   		gpio_put(16,0);
   		sleep_us(100);
	}

}

#include <stdio.h>
#include "pico/stdlib.h"

/*#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    gpio_init(16);
    gpio_set_dir(16, GPIO_OUT);
    gpio_put(16,1);
    
    }
#endif*/

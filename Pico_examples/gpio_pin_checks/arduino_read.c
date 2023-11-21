#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <time.h>

#define ENCODER_PIN 21

void set_up() {
	stdio_init_all();

	// Set up GPIO pins
 	gpio_init(ENCODER_PIN);
    	gpio_set_dir(ENCODER_PIN, GPIO_OUT);

 	sleep_ms(30000); //0.5 min delay for putty
	printf("Hello, multicore!\n");
}

int main(){
	/// set up() 
	set_up();
//core 0
    /// \tag::setup_multicore[]
	while(1) {
		gpio_put(ENCODER_PIN,1);
		gpio_put(ENCODER_PIN,0);
	}	
}

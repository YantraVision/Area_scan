#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include <time.h>

#define ENCODER_PIN 21
#define TRIGGER_ENABLE_PIN 19
#define PROXY_READ_PIN 26
#define OUT_2 22
#define OUT_3 27

int Sample_count=10;
int proxy_samples = 5;
int encoder_count = 0;
int RunLength =500;
int PulseWidth = 5;
int Slip = 0;
int StartPos = 250;

void set_up() {
	stdio_init_all();

	// Set up GPIO pins
 	gpio_init(ENCODER_PIN);
    	gpio_set_dir(ENCODER_PIN, GPIO_IN);

	gpio_init(TRIGGER_ENABLE_PIN);
	gpio_set_dir(TRIGGER_ENABLE_PIN, GPIO_OUT);
	gpio_put(TRIGGER_ENABLE_PIN, 0); // Set trigger enable pin low initially

	//gpio_init(PROXY_READ_PIN);
	gpio_init(OUT_2);
	gpio_init(OUT_3);
	//gpio_set_dir(PROXY_READ_PIN, GPIO_IN);
	gpio_set_dir(OUT_2, GPIO_OUT);
	gpio_set_dir(OUT_3, GPIO_OUT);
	//gpio_put(PROXY_READ_PIN, 0);
	gpio_put(OUT_2, 0);
	gpio_put(OUT_3, 0);
	
    	sleep_ms(30000); //0.5 min delay for putty
	printf("Hello, multicore!\n");
	//int num =0;printf("enter num: ");scanf("%d\n",&num);printf("%d\n",num);
}

void core1_entry() {
	//core 1
	uint32_t encoder_in = 0;
	uint32_t encoder_count = 0, level_reader = 0;
	bool g=0, E_high =0, E_low =0, E_rise =0, E_fall =0;

	while(1) {
		g = gpio_get(ENCODER_PIN);
		//multicore_fifo_push_blocking(g);
		encoder_in = encoder_in << 1;
		encoder_in |= g;

		switch(encoder_in) {
			case 0 :		//0 ... 0
				E_low = 1;
				E_high = 0;
				E_rise = 0;
				E_fall = 0;
				level_reader += 1;
				break;
			case 65535:	//case 3:	//case 31 :		//0..0 1..1 - 2^16 - 1
				E_rise = 1;
				E_low = 0;
				E_high = 0;
				E_fall = 0;
				encoder_count += 1;
				int level_count =0;	
				if(multicore_fifo_wready()) {
					//level_count = level_reader;
					//if(encoder_count == 25 || encoder_count == 50 || encoder_count ==100) {
					multicore_fifo_push_blocking(encoder_count);
					//multicore_fifo_push_blocking(level_count);
					//}
				}
				//level_reader = 0;
				if (encoder_count >= (StartPos+ Slip) && encoder_count < (StartPos+ Slip+ PulseWidth)) {
		        
        				        gpio_put(TRIGGER_ENABLE_PIN, 1); // Enable trigger
        				        gpio_put(OUT_2, 1); // Enable trigger
        				        gpio_put(OUT_3,1); // Enable trigger
        	    		} 
		    		else {
        	    		    gpio_put(TRIGGER_ENABLE_PIN, 0); // Disable trigger
		    		    gpio_put(OUT_2, 0); 
        	    		    gpio_put(OUT_3,0);
        	   		}
				//reset condition Encoder revolution - 500
				/*if(encoder_count == RunLength) {
					encoder_count = 0; 
					E_high =0, E_low =0, E_rise =0, E_fall =0, level_reader = 0;
				}*/
				break;
			case 4294967295:		//case 7:	//case 1023 :	//1 ... 1 - 2^32 - 1
				E_high = 1;
				E_rise = 0;
				E_low = 0;
				E_fall = 0;
				level_reader += 1;
				break;
			case 4294901760:	//case 24:	//case 992:	//1..1 0..0 - 2^31+..+2^16
				E_fall = 1;
				E_rise = 0;
				E_low = 0;
				E_high = 0;
				break;
		}

		/*if(E_rise ) {	//rising edge
			encoder_count += 1;
			int level_count =0;	
			if(multicore_fifo_wready()) {
				level_count = level_reader;
				multicore_fifo_push_blocking(level_count);
			}
			level_reader = 0;
			if (encoder_count >= (StartPos+ Slip) && encoder_count < (StartPos+ Slip+ PulseWidth)) {
	        
        			        gpio_put(TRIGGER_ENABLE_PIN, 1); // Enable trigger
        			        gpio_put(OUT_2, 1); // Enable trigger
        			        gpio_put(OUT_3,1); // Enable trigger
            		} 
	    		else {
            		    gpio_put(TRIGGER_ENABLE_PIN, 0); // Disable trigger
	    		    gpio_put(OUT_2, 0); 
            		    gpio_put(OUT_3,0);
           		}
		}
		else if (E_high || E_low) {
			level_reader += 1;
		}*/

		//reset condition Encoder revolution - 500
		if(encoder_count == RunLength) {
			encoder_count = 0; 
			E_high =0, E_low =0, E_rise =0, E_fall =0;//, level_reader = 0;
		}
		
	}
}

//main function
int main() {
	set_up();	//setup GPIO pins

	//core 0 - debug core
	
    	multicore_launch_core1(core1_entry);	//launch core 1
	while(1) {
		if(multicore_fifo_rvalid()) {
			uint32_t data_in = multicore_fifo_pop_blocking();
			printf("value from core 1 was %d\n", data_in);
		}
	}
}

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <time.h>

#define ENCODER_PIN 21
#define TRIGGER_ENABLE_PIN 19
#define PROXY_READ_PIN 26
#define OUT_2 22
#define OUT_3 27
//typedef uint64_t absolute_time_t;
int Sample_count=10;
int proxy_samples = 5;
int encoder_count = 0;
int RunLength =500;
int PulseWidth = 5;
int Slip = 0;
int StartPos = 250;
uint64_t start,end,start_time, end_time;
static int64_t process_time;
/*uint64_t start_cycle, end_cycle;

clock_t clock()
{
    return (clock_t) time_us_64() ;
}
*/

static uint64_t get_time(void) {
    // Reading low latches the high value
    uint32_t lo = timer_hw->timelr;
    uint32_t hi = timer_hw->timehr;
    return ((uint64_t) hi << 32u) | lo;
}
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
}

void core1_entry() {
	//core 1
	uint32_t encoder_in = 0;
	uint32_t encoder_count = 0, level_reader = 0;
	bool g=0, E_rise =0, E_fall =0;
	//uint32_t toggle_pin =0;

	while(1) {
        //gpio_put(TRIGGER_ENABLE_PIN, toggle_pin); // Disable trigger
	//printf("%d\n",toggle_pin);
	//clock_t start_tick = clock();
	start_time = get_time();
		g = gpio_get(ENCODER_PIN);
		//multicore_fifo_push_blocking(g);
		encoder_in = encoder_in << 1;
		encoder_in |= g;

		switch(encoder_in) {
			case 255:	//case 3:	//case 31 :		//0..0 1..1 - 2^16 - 1
				E_rise = 1;
				E_fall = 0;
				encoder_count += 1;
				if (encoder_count >= (StartPos+ Slip) && encoder_count < (StartPos+ Slip+ PulseWidth)) {
		        
        				        //gpio_put(TRIGGER_ENABLE_PIN, 1);
						gpio_put(OUT_2, 1); // Enable trigger
        				        gpio_put(OUT_3,1); // Enable trigger
        	    		} 
		    		else {
		    		    //gpio_put(TRIGGER_ENABLE_PIN, 0);
				    gpio_put(OUT_2, 0); 
        	    		    gpio_put(OUT_3,0);
        	   		}
				break;
			case 65280:		//case 7:	//case 1023 :	//1 ... 1 - 2^32 - 1
				E_rise = 0;
				E_fall = 1;
				break;
			
		}

		

		//reset condition Encoder revolution - 500
		if(encoder_count == RunLength) {
			encoder_count = 0; 
			E_rise =0, E_fall =0;//, level_reader = 0;
		}
		//toggle_pin = ~toggle_pin;
		
	//clock_t end_tick = clock();
	end_time = get_time();
	if(multicore_fifo_wready()) {
		multicore_fifo_push_blocking(start_time);
		multicore_fifo_push_blocking(end_time);
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
			int64_t data_in;
			clock_t start, end ;
			start = multicore_fifo_pop_blocking();
			end = multicore_fifo_pop_blocking();
			double time_diff = end - start;
			uint64_t cycles_count = time_diff *133;
			printf("%lu %lu %f %llu \n", start, end, time_diff, cycles_count);
		}
	}
}

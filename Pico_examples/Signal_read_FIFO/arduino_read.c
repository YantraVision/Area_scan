/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <time.h>

#define ENCODER_PIN 18
#define TRIGGER_ENABLE_PIN 20
#define FIFO_READ_PIN 27

clock_t Es_time;
int encoder_count = 0;

void set_up() {
	stdio_init_all();

	// Set up GPIO pins
 	gpio_init(ENCODER_PIN);
    	gpio_set_dir(ENCODER_PIN, GPIO_IN);

	gpio_init(TRIGGER_ENABLE_PIN);
	gpio_set_dir(TRIGGER_ENABLE_PIN, GPIO_OUT);
	gpio_put(TRIGGER_ENABLE_PIN, 0); // Set trigger enable pin low initially

	gpio_init(FIFO_READ_PIN);
	gpio_set_dir(FIFO_READ_PIN, GPIO_OUT);
	gpio_put(FIFO_READ_PIN, 0);

    	sleep_ms(30000); //0.5 min delay for putty
	printf("Hello, multicore!\n");
}

void core1_entry() {
	//core 1
	uint32_t g=0;
	uint8_t array_a[8] = {0};
    	uint8_t currStateOfPin = 0;
	uint8_t nextStateOfPin = 0;
	uint8_t curVal=0;

	while(1) {
	if (multicore_fifo_rvalid()){
			g = multicore_fifo_pop_blocking();
			gpio_put(FIFO_READ_PIN,g);
			//printf("Value at Encoder pin - %d\n",g);
		}
	
	//printf("value at pin 5 %d\n",array_a[4]);
        array_a[0] = array_a[1];
        array_a[1] = array_a[2];
        array_a[2] = array_a[3];
        array_a[3] = array_a[4];
	array_a[4] = array_a[5];
        array_a[5] = array_a[6];
        array_a[6] = array_a[7];
        array_a[7] = g;
	
	//rising edge
        if (currStateOfPin == 0) {
            nextStateOfPin =  array_a[7] & array_a[6] & array_a[5] & array_a[4] & array_a[3] & array_a[2] & array_a[1] & array_a[0];
            //printf("rised \n");
        }
	//debounce
        if (currStateOfPin == 1) {
            nextStateOfPin =  array_a[7] | array_a[6] | array_a[5] | array_a[4] | array_a[3] | array_a[2] | array_a[1] | array_a[0];
        }
        
	if (currStateOfPin == 0 && nextStateOfPin == 1) {
            //printf("entered \n");
            
	    encoder_count = encoder_count + 1;
	    

            if (encoder_count >= 250 && encoder_count < 251) {
                gpio_put(TRIGGER_ENABLE_PIN, 1); // Enable trigger
		//printf("entered \n");
            } 
	    else {
                gpio_put(TRIGGER_ENABLE_PIN, 0); // Disable trigger
            }
	    
        } 
        currStateOfPin = nextStateOfPin;
	
        if (encoder_count == 500) { //reset  
            encoder_count = 0;
	    //gpio_put(TRIGGER_ENABLE_PIN, 1);
             // Disable trigger
            //printf("reseted \n");
        }
	//else { gpio_put(TRIGGER_ENABLE_PIN, 0); }
	}
}

int main(){
	/// set up() 
	set_up();
//core 0
    /// \tag::setup_multicore[]
    	multicore_launch_core1(core1_entry);
	
	//uint8_t curVal =0;//int count=0;
	while(1) {
		uint8_t curVal = gpio_get(ENCODER_PIN);//count++;
		if (multicore_fifo_wready()){
			multicore_fifo_push_blocking(curVal);
		}
		
	}
}

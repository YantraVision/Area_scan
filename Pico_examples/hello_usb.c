/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"

#define ENCODER_PIN 13
#define LED_PIN 16
#define TRIGGER_ENABLE_PIN 27

int encoder_count = 0;

void setup() {
    // Initialize the Raspberry Pi Pico SDK
    stdio_init_all();

    // Set up GPIO pins
    gpio_init(ENCODER_PIN);
    gpio_set_dir(ENCODER_PIN, GPIO_IN);
    
    //gpio_pull_up(ENCODER_PIN); // Enable pull-up resistor
    //gpio_put(ENCODER_PIN, 0); // Set encoder enable pin low initially
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(TRIGGER_ENABLE_PIN);
    gpio_set_dir(TRIGGER_ENABLE_PIN, GPIO_OUT);
    gpio_put(TRIGGER_ENABLE_PIN, 0); // Set trigger enable pin low initially
}

void encoder_st() {
    uint8_t array_a[5] = {0};
    uint8_t currStateOfPin = 0;
    uint8_t nextStateOfPin = 0;
    uint8_t curVal=0;

    for (;;) {
        curVal = gpio_get(ENCODER_PIN) ;
	//printf("%d",array_a[0]);
        array_a[0] = array_a[1];
        array_a[1] = array_a[2];
        array_a[2] = array_a[3];
        array_a[3] = array_a[4];
        array_a[4] = curVal;
	
	//rising edge
        if (currStateOfPin == 0) {
            nextStateOfPin = array_a[4] & array_a[3] & array_a[2] & array_a[1] & array_a[0];
            //printf("rised \n");
        }
        if (currStateOfPin == 1) {
            nextStateOfPin = array_a[4] | array_a[3] | array_a[2] | array_a[1] | array_a[0];
        }
        
	if (currStateOfPin == 0 && nextStateOfPin == 1) {
            //printf("entered \n");
            encoder_count = encoder_count + 1;

            if (encoder_count >= 510 && encoder_count < 520) {
                gpio_put(TRIGGER_ENABLE_PIN, 1); // Enable trigger
            } 
	    else {
                gpio_put(TRIGGER_ENABLE_PIN, 0); // Disable trigger
		gpio_put(LED_PIN,0);
            }
        }
        currStateOfPin = nextStateOfPin;
        if (encoder_count == 1024) {
            encoder_count = 0;
            gpio_put(TRIGGER_ENABLE_PIN, 0); // Disable trigger
            //printf("reseted \n");
	    gpio_put(LED_PIN,1);
        }
    }
}

int main(void) {
    setup();
    while (1) {
        encoder_st();
    }
    return 0;
}


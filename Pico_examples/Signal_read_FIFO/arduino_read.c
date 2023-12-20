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

#define __PRINTS__
int Sample_count=10;
int proxy_samples = 5;
int encoder_count = 0;
int RunLength =500;
int PulseWidth = 5;
int Slip = 0;
int StartPos = 250;
uint64_t start_time,end_time, start_array[5],end_array[5];
bool thread_ready=0;
void set_up() {
	stdio_init_all();

	// Set up GPIO pins
 	gpio_init(ENCODER_PIN);
    	gpio_set_dir(ENCODER_PIN, GPIO_IN);

	gpio_init(TRIGGER_ENABLE_PIN);
	gpio_set_dir(TRIGGER_ENABLE_PIN, GPIO_OUT);
	gpio_put(TRIGGER_ENABLE_PIN, 0); // Set trigger enable pin low initially

	gpio_init(PROXY_READ_PIN);
	gpio_init(OUT_2);
	gpio_init(OUT_3);
	gpio_set_dir(PROXY_READ_PIN, GPIO_IN);
	gpio_set_dir(OUT_2, GPIO_OUT);
	gpio_set_dir(OUT_3, GPIO_OUT);
	gpio_put(PROXY_READ_PIN, 0);
	gpio_put(OUT_2, 0);
	gpio_put(OUT_3, 0);
	
    	sleep_ms(30000); //0.5 min delay for putty
	printf("Hello, multicore!\n");
}

void core1_entry() {
	//core 1
	uint32_t g=0,count=0,p=0;
	int array_a[Sample_count],array_p[proxy_samples];
    	uint8_t currStateOfPin = 0;
	uint8_t nextStateOfPin = 0;
	uint8_t proxy_currState = 0;
	uint8_t proxy_nextState = 0;
	bool E_rise =0, E_fall=0,E_high=0, E_low=0,rise =0, fall=0,high=0, low=0;
	uint8_t curVal=0;
	
	//int64_t process_time;
	//uint32_t pos_edges =0, neg_edges =0;

	while(1) {
		
		#ifdef __PRINTS__
		//printf("thread status core 1 - %d\n",thread_ready);
		if(thread_ready == 0) {
		#endif
			g = gpio_get(ENCODER_PIN);
			p = gpio_get(PROXY_READ_PIN);
			//multicore_fifo_push_blocking(g);
			//printf("value at p %d\n",p);
			//printf("value at g%d\n",g);
	
			for(int k=0;k<proxy_samples;k++){
				array_p[proxy_samples-k] = array_p[proxy_samples-(k+1)];
		        }
	        	array_p[0] = p;
	        	bool proxy_detect = 1;
		        for(int i=0; i<proxy_samples; i++) {
 				proxy_detect &= array_p[i];
		        }
			proxy_nextState = proxy_detect;
       
			for(int i=1; i<Sample_count; i++) {
	 			array_a[Sample_count-i] = array_a[Sample_count-(i+1)];
			      //array_p[Sample_count-i] = array_p[Sample_count-(i+1)];
			}
			array_a[0] = g;

			#ifdef __PRINTS__
			if(count!=0) { //reads every g state cycle
		 
				//end_time = get_absolute_time();
				end_array[count] = get_absolute_time();
				start_array[count] = start_time;
				multicore_fifo_push_blocking(start_array[count]);
				multicore_fifo_push_blocking(end_array[count]);
				thread_ready = 1;
				count = 0;
				//printf("%d\n",thread_ready);
				//printf("%d\n",start_time);
				//printf("%d\n",end_time);
		
			}
			start_time = get_absolute_time();
			count+=1;
			#endif
      
			bool detect_and = 1, detect_or = 1;
		        for(int j=0; j<Sample_count; j++) {
				detect_and &= array_a[j];
			      //proxy_detect &= array_p[i];
			      //detect_or |= array_a[j];
			}
			nextStateOfPin = detect_and;
		       //printf("%d %d\n",currStateOfPin,nextStateOfPin);
	
			
	
			int choice = (	proxy_currState << 1) | proxy_nextState ;
			//multicore_fifo_push_blocking(choice);
			switch (choice) {
				case 0:
					low = 1; break;
				case 1:
					rise = 1; break;
				case 2:
					fall = 1 ;break;
				case 3:
					high = 1; break;
			}
			//rising edge
 	
		       int E_choice = (	currStateOfPin << 1) | nextStateOfPin ;
		       //printf("%d %d %d\n",currStateOfPin,nextStateOfPin,E_choice);
			//multicore_fifo_push_blocking(E_choice);
			switch (E_choice) {
				case 0:
					E_low = 1; break;
				case 1:
					E_rise = 1; break;
				case 2:
					E_fall = 1 ;break;
				case 3:
					E_high = 1; break;
			}
			
			//if (currStateOfPin == 0 && nextStateOfPin == 1) {
			if (E_rise==1 ){ //&& E_fall ==1 && E_high==1 && E_low ==0) {
			
            			E_rise =0; E_fall=0; E_high=0; E_low=0;
				encoder_count = encoder_count + 1;
            			//printf("entered %d\n", encoder_count);
            			//printf("value with rise and fall count %d, %d %d\n", rise,fall, encoder_count);
	    			//if (multicore_fifo_wready()){
	    				//multicore_fifo_push_blocking(rise);
	     				//multicore_fifo_push_blocking(encoder_count);
	   			//}
	    			//pos_edges=0;neg_edges=0;
            			if (encoder_count >= (StartPos+ Slip) && encoder_count < (StartPos+ Slip+ PulseWidth)) {
	        
        			        gpio_put(TRIGGER_ENABLE_PIN, 1); // Enable trigger
        			        gpio_put(OUT_2, 1); // Enable trigger
        			        gpio_put(OUT_3,1); // Enable trigger
					//printf("*********** triggered *************\n");
            			} 
	    			else {
            			    gpio_put(TRIGGER_ENABLE_PIN, 0); // Disable trigger
	    			    gpio_put(OUT_2, 0); 
            			    gpio_put(OUT_3,0);
            			}
	    
        		} 
        		currStateOfPin = nextStateOfPin;
        		proxy_currState = proxy_nextState;
	
         		if ( (rise ==1 && fall ==1 && high==1 ) || encoder_count == RunLength) {
		 	//if (encoder_count == RunLength) { //reset  
        		//if ( encoder_count == RunLength) { //reset  
           		// printf("reseted with rise and fall count %d, %d %d\n", rise,fall, encoder_count);
	    			encoder_count = 0 ; rise =0; fall=0; high=0; low=0;
	    
	    			//pos_edges=0;neg_edges=0;
	    			//gpio_put(TRIGGER_ENABLE_PIN, 1);
             			// Disable trigger
        		}
		
	
			//gpio_put(OUT_2,0);

			//(encoder_count == RunLength) { encoder_count =0;}
			//else { gpio_put(TRIGGER_ENABLE_PIN, 0); }
			//static int64_t time_diff ;
			//float process_time_us = process_time/1000;
			//printf("start %ld end %ld Time difference in a cycle %.3f %lu\n",start_time, end_time,process_time_us, process_time); //us
		#ifdef __PRINTS__
			}
		else {
			continue;
		}

		#endif
	}
}

int main(){
	/// set up() 
	set_up();
//core 0
    /// \tag::setup_multicore[]
    	multicore_launch_core1(core1_entry);
	int64_t process_time;	
	//uint8_t curVal =0;//int count=0;
	while(1) {
	//	reads pos and neg edge count
		#ifdef __PRINTS__
		//printf("thread status core 0 - %d\n",thread_ready);
		if(thread_ready == 1) {
			if (multicore_fifo_rvalid()){
				//int p = multicore_fifo_pop_blocking();
				//int ch = multicore_fifo_pop_blocking();
				//int c = multicore_fifo_pop_blocking();
				//printf("%d\n",thread_ready);
				uint32_t time_value_s = multicore_fifo_pop_blocking();
				uint32_t time_value_e = multicore_fifo_pop_blocking();
				//process_time =  (time_value_e - time_value_s) ; //absolute_time_diff_us(time_value_s, time_value_e); 
				//printf("Value at g value - %d \n", p);
				//printf("Value at encoder count - %d \n", c);
				printf("Value at time diff- %lu %lu %lu\n", time_value_s, time_value_e,(time_value_e - time_value_s));
				//printf("Value at choice  %d\n",ch );
		
				thread_ready = 0;
			}
		}
		else {
			continue;
		}
		#endif	
		
	} 
}

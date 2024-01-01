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
	uint32_t g=0,count=0,p=0,encoder_in = 0,proxy_in = 0;
	int array_a[Sample_count],array_p[proxy_samples];
    	uint8_t currStateOfPin = 0;
	uint8_t nextStateOfPin = 0;
	uint8_t proxy_currState = 0;
	uint8_t proxy_nextState = 0;
	bool E_rise =0, E_fall=0,E_high=0, E_low=0,P_rise =0, P_fall=0,P_high=0, P_low=0;
	bool init =0;
	uint8_t curVal=0;
	
	while(1) {
		
		#ifdef __PRINTS__
		printf("thread status core 1 - %d\n",thread_ready);
		if(thread_ready == 0) {
		#endif
			g = gpio_get(ENCODER_PIN);
			p = gpio_get(PROXY_READ_PIN);
	
			/*for(int k=0;k<proxy_samples;k++){
				array_p[proxy_samples-k] = array_p[proxy_samples-(k+1)];
		        }
	        	array_p[0] = p;
			bool proxy_detect = 1;
		        for(int i=0; i<proxy_samples; i++) {
 				proxy_detect &= array_p[i];
		        }
			proxy_nextState = proxy_detect;
			*/
			if(!init){
				proxy_currState = p;
				currStateOfPin = g;
				init =1;
			}
			proxy_in = proxy_in << 1;
			proxy_in |= p;
			if(proxy_currState == 0){
				if((proxy_in & 31) == 31){
					proxy_nextState =1;
				}
				else {
					proxy_nextState = 0;
				}
			}
			else {	//proxy_currState == 1
				if((proxy_in & 31) > 0) {	//debounce if 1 allowed 0's  >= 15
					proxy_nextState = 1;
				}
				else {
					proxy_nextState = 0;
				}
			}
       	
       
			/*for(int i=1; i<Sample_count; i++) {
	 			array_a[Sample_count-i] = array_a[Sample_count-(i+1)];
			}
			array_a[0] = g;

			#ifdef __PRINTS__
			if(count!=0) { //reads every g state cycle
				end_array[count] = get_absolute_time();
				start_array[count] = start_time;
				multicore_fifo_push_blocking(start_array[count]);
				multicore_fifo_push_blocking(end_array[count]);
				thread_ready = 1;
				count = 0;
			}
			start_time = get_absolute_time();
			count+=1;
			#endif
      
			bool detect_and = 1, detect_or = 1;
		        for(int j=0; j<Sample_count; j++) {
				detect_and &= array_a[j];
			}
			nextStateOfPin = detect_and;
			*/

			encoder_in = encoder_in << 1;
			encoder_in |= g;
			if(currStateOfPin == 0){
				if((encoder_in & 1023) == 1023){
					nextStateOfPin =1;
				}
				else {
					nextStateOfPin = 0;
				}
			}
			else {	//proxy_currState == 1
				if((encoder_in & 1023) > 0) {	//debounce if 1 allowed 0's  >= 511
					nextStateOfPin = 1;
				}
				else {
					nextStateOfPin = 0;
				}
			}

			int choice = (	proxy_currState << 1) | proxy_nextState ;
			switch (choice) {
				case 0:
					P_low = 1; 
					/*P_rise =0;
					P_fall = 0;
					P_high = 0;*/
					break;
				case 1:
					P_rise = 1;
				       	/*P_fall = 0;
					P_low = 0;*/
					break;
				case 2:
					P_fall = 1 ;
					/*P_rise = 0;
					P_high = 0;*/
					break;
				case 3:
					P_high = 1;
				       	/*P_fall = 0;
					P_low = 0;*/
					break;
			}
			//rising edge
 	
		       int E_choice = (	currStateOfPin << 1) | nextStateOfPin ;
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
			if (E_rise==1 ){ 
			
            			E_rise =0; E_fall=0; E_high=0; E_low=0;
				encoder_count = encoder_count + 1;
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
        		currStateOfPin = nextStateOfPin;
        		proxy_currState = proxy_nextState;
	
         		if ( (P_rise ==1 && P_fall ==1 && P_high==1 ) || encoder_count == RunLength) {
	    			encoder_count = 0 ; P_rise =0; P_fall=0; P_high=0; P_low=0;
        		}
		
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
	while(1) {
	//	reads pos and neg edge count
		#ifdef __PRINTS__
		printf("thread status core 0 - %d\n",thread_ready);
		if(thread_ready == 1) {
			if (multicore_fifo_rvalid()){
				uint32_t time_value_s = multicore_fifo_pop_blocking();
				uint32_t time_value_e = multicore_fifo_pop_blocking();
				printf("Value at time diff- %lu %lu %lu\n", time_value_s, time_value_e,(time_value_e - time_value_s));
				thread_ready = 0;
			}
		}
		else {
			continue;
		}
		#endif	
		
	} 
}

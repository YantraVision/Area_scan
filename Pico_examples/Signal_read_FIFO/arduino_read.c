#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <time.h>

#define ENCODER_PIN 26
#define TRIGGER_ENABLE_PIN 19 
#define PROXY_READ_PIN 21
#define PROXY_WRITE_PIN 22

int Sample_count=20;
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

	gpio_init(PROXY_READ_PIN);
	gpio_init(PROXY_WRITE_PIN);
	gpio_set_dir(PROXY_READ_PIN, GPIO_IN);
	gpio_set_dir(PROXY_WRITE_PIN, GPIO_OUT);
	gpio_put(PROXY_READ_PIN, 0);
	gpio_put(PROXY_WRITE_PIN, 0);

    	sleep_ms(30000); //0.5 min delay for putty
	printf("Hello, multicore!\n");
}

void core1_entry() {
	//core 1
	uint32_t g=0, p=0;
	int array_a[Sample_count], array_p[proxy_samples];
    	uint8_t currStateOfPin = 0;
    	uint8_t proxy_currState = 0;
	uint8_t nextStateOfPin = 0;
	uint8_t proxy_nextState = 0;
	bool rise =0, fall=0,high=0, low=0;
	uint8_t curVal=0;
	//uint32_t pos_edges =0, neg_edges =0;

	while(1) {
	g = gpio_get(ENCODER_PIN);
	p = gpio_get(PROXY_READ_PIN);
//	printf("value at p %d\n",p);
	//printf("value at g%d\n",g);
       for(int i=1; i<Sample_count; i++) {
	      array_a[Sample_count-i] = array_a[Sample_count-(i+1)];
	      //array_p[Sample_count-i] = array_p[Sample_count-(i+1)];
       }
      array_a[0] = g;
      
      for(int k=0;k<proxy_samples;k++){
	      array_p[proxy_samples-k] = array_p[proxy_samples-(k+1)];
      }
      array_p[0] = p;
      bool proxy_detect = 1;
      for(int i=0; i<proxy_samples; i++) {
	      proxy_detect &= array_p[i];
      }
      
      bool detect_and = 1, detect_or = 1;
      for(int j=0; j<Sample_count; j++) {
	      detect_and &= array_a[j];
	      //proxy_detect &= array_p[i];
	      //detect_or |= array_a[j];
      }
	
	/*if(g == 1) { pos_edges++;}
	else { neg_edges++; }*/
	
      //rising edge
      if (currStateOfPin == 0 || currStateOfPin == 1) {
           nextStateOfPin = detect_and;
          // printf("rised \n");
      }

	if (proxy_currState ==0 || proxy_currState ==1) {
		proxy_nextState = proxy_detect;
		//printf("P curr state 0 next : %d %d\n",proxy_currState,proxy_nextState);
	}
	/*if(proxy_currState ==1) {
		proxy_nextState = proxy_detect;
		printf("P curr state 1 next : %d %d\n",proxy_currState,proxy_nextState);
	}
	//debounce
        if (currStateOfPin == 1) {
            nextStateOfPin =  detect_and;
       }
       */
       if(proxy_currState == 0 && proxy_nextState ==1) {
          rise =1;//printf("rise\n");
       }//else {rise =0;}	  
       if(proxy_currState ==1 && proxy_nextState ==0) {
          fall =1;//printf("fall\n");
       } //else {fall =0; }

       if(proxy_currState == 1 && proxy_nextState ==1) {
          high =1;//printf("high state\n");
       }//else {rise =0;}	  
       if(proxy_currState ==0 && proxy_nextState ==0) {
          low =1;//printf("low state\n");
       }
       
	if (currStateOfPin == 0 && nextStateOfPin == 1) {
            //printf("entered \n");
            
	    encoder_count = encoder_count + 1;
            //printf("value with rise and fall count %d, %d %d\n", rise,fall, encoder_count);
	    if (multicore_fifo_wready()){
	    	//multicore_fifo_push_blocking(rise);
	    	//multicore_fifo_push_blocking(fall);
	    	multicore_fifo_push_blocking(encoder_count);
	    }
	    //pos_edges=0;neg_edges=0;
            if (encoder_count >= (StartPos+ Slip) && encoder_count < (StartPos+ Slip+ PulseWidth)) {
                gpio_put(TRIGGER_ENABLE_PIN, 1); // Enable trigger
		//printf("*********** triggered *************\n");
            } 
	    else {
                gpio_put(TRIGGER_ENABLE_PIN, 0); // Disable trigger
            }
	    
        } 
        currStateOfPin = nextStateOfPin;
        proxy_currState = proxy_nextState;
	
        if ( (rise ==1 && fall ==1 && high==1 && low==0 ) || encoder_count == RunLength) { //reset  
        //if ( encoder_count == RunLength) { //reset  
           // printf("reseted with rise and fall count %d, %d %d\n", rise,fall, encoder_count);
            gpio_put(PROXY_WRITE_PIN,1);
	    encoder_count = 0 ; rise =0; fall=0; high=0; low=0;
	    gpio_put(PROXY_WRITE_PIN,0) ;
	    //pos_edges=0;neg_edges=0;
	    //gpio_put(TRIGGER_ENABLE_PIN, 1);
             // Disable trigger
        }
	//gpio_put(PROXY_WRITE_PIN,0);

	//(encoder_count == RunLength) { encoder_count =0;}
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
	//	reads pos and neg edge count
		if (multicore_fifo_rvalid()){
			//int p = multicore_fifo_pop_blocking();
			//int n = multicore_fifo_pop_blocking();
			int c = multicore_fifo_pop_blocking();
			printf("Value at encoder count - %d\n",c);
		}	
		
	} 
}

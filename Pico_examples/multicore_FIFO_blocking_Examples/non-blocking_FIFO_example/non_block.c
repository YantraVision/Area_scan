#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#define FLAG_VALUE 123
#define FLA_VALUE 12

void core1_entry() {
    uint32_t g =0;
    while(1) {
	if (multicore_fifo_wready()){
		multicore_fifo_push_blocking(FLA_VALUE);
	}
    	 if (multicore_fifo_rvalid()) {
		 g = multicore_fifo_pop_blocking();
	}
    	if (g != FLAG_VALUE)
        	printf("Hmm, that's not right on core 1!\n");
    	else {
        	printf("Its all gone well on core 1! %d\n",g);
		if (multicore_fifo_wready()){
			multicore_fifo_push_blocking(FLA_VALUE);
		}
	}
      }
   	while (1)
        	tight_loop_contents();
    
}

int main() {
    stdio_init_all();
    sleep_ms(60000); //1 min delay for putty
    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);
    uint32_t g = 0;
    // Wait for it to start up
    while(1) {
	 if (multicore_fifo_rvalid()) {
		 g = multicore_fifo_pop_blocking();
	}
    	if (g != FLA_VALUE)
    		printf("Hmm, that's not right on core 0!\n");
    	else {
    		printf("It's all gone well on core 0! %d\n",g);
    		 if (multicore_fifo_wready()) {
			 multicore_fifo_push_blocking(FLAG_VALUE);
		 }
    	}
    /// \end::setup_multicore[]
    }
}

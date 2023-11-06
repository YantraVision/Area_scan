if (TARGET tinyusb_device)
	add_executable(multicore_fifo_irqs
	        multicore_fifo_irqs.c
	        )
	
	target_link_libraries(multicore_fifo_irqs
	        pico_multicore
	        pico_stdlib)
	
	# create map/bin/hex file etc.
	pico_add_extra_outputs(multicore_fifo_irqs)
	
	# add url via pico_set_program_url
	example_auto_set_url(multicore_fifo_irqs)
	# enable usb output, disable uart output
	   	pico_enable_stdio_usb(multicore_fifo_irqs 1)
	    	pico_enable_stdio_uart(multicore_fifo_irqs 0)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

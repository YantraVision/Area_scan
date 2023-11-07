if (TARGET tinyusb_device)
	add_executable(non_block
	        non_block.c
	        )
	
	target_link_libraries(non_block
	        pico_multicore
	        pico_stdlib)
	
	# create map/bin/hex file etc.
	pico_add_extra_outputs(non_block)
	
	# add url via pico_set_program_url
	example_auto_set_url(non_block)
	# enable usb output, disable uart output
	   	pico_enable_stdio_usb(non_block 1)
	    	pico_enable_stdio_uart(non_block 0)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

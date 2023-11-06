if (TARGET tinyusb_device)
	add_executable(multicore_runner_queue
      		multicore_runner_queue.c
        	)

	target_link_libraries(multicore_runner_queue
        	pico_multicore
        	pico_stdlib)

	# create map/bin/hex file etc.
	pico_add_extra_outputs(multicore_runner_queue)

	# add url via pico_set_program_url
	example_auto_set_url(multicore_runner_queue)

	# enable usb output, disable uart output
	pico_enable_stdio_usb(multicore_runner_queue 1)
	pico_enable_stdio_uart(multicore_runner_queue 0)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

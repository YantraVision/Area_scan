cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
#include(example_auto_set_url.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(gpio_pin_checks
        arduino_read.c
        )

	# Add pico_multicore which is required for multicore functionality
	target_link_libraries(gpio_pin_checks
	        pico_stdlib
	        pico_multicore
		pico_time)
	
	# create map/bin/hex file etc.
	pico_add_extra_outputs(gpio_pin_checks)
	
	# add url via pico_set_program_url
	#example_auto_set_url(hello_multicore)

	
   	# enable usb output, disable uart output
   	pico_enable_stdio_usb(gpio_pin_checks 1)
    	pico_enable_stdio_uart(gpio_pin_checks 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

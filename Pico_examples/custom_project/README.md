# CREATE OWN PROJECT

Create a custom directory and move to it.
```
mkdir custom_project
cd !$
```
Generate .c file (multicore.c)
Create CmakeLists.txt as below
```
cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
if (TARGET tinyusb_device)
	add_executable(hello_multicore
        multicore.c
        )

	# Add pico_multicore which is required for multicore functionality
	target_link_libraries(hello_multicore
	        pico_stdlib
	        pico_multicore)
	
	# create map/bin/hex file etc.
	pico_add_extra_outputs(hello_multicore)
	
   	# enable usb output, disable uart output
   	pico_enable_stdio_usb(hello_multicore 1)
    	pico_enable_stdio_uart(hello_multicore 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

```
Copy `pico_sdk_import.cmake` to your working directory
```
cp /home/rpf/pico-setup/pico/pico-examples/pico_sdk_import.cmake .
```
Set `PICO_SDK_PATH`
```
export PICO_SDK_PATH=/home/rpf/pico-setup/pico/pico-sdk
```
Create `build` directory and move to that
```
mkdir build
cd build
```
Configure sdk
```
cmake .. 
```
builds the design
```
make -j4 
```
Connect PICO module to PC and **do BOOTSEL bootmode**
Get connected PICO Port with **dmesg** command
Mount PICO
``` 
sudo mount /dev/sdb1 /mnt/pico/
```
Copy .uf2 file to PICO , it completes booting, unmount and runs the uploaded code automatically
```
sudo cp hello_multicore.uf2 /mnt/pico
```




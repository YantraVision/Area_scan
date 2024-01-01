# COMPILE PROJECT
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
```

# RUN PROJECT

## Hardware booting
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
#Software booting
Using `openocd` to run and debug the project source file. It configures rpi and runs the elf file.
```
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program ./gpio_pin_checks.elf verify reset exit"
```




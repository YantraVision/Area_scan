
**Raspberry pi user name**: 192.168.90.72

**password**: pi

go to pico-examples and choose appropriate example directory you need to work 

    cd /home/rpf/pico-setup/pico/pico-examples/

    eg: vim /home/rpf/pico-setup/pico/pico-examples/blink/blink.c

once your code is completed
go to build and appropriate example directory to get complie the code you generated, 
        
    cd /home/rpf/pico-setup/pico/pico-examples/build/blink

run 

    make -j4
Now, connect the PICO module to pc
,**hold BOOTSEL button while getting power it up**

now it will act as storage module

check dmesg, find which port pico is connected

eg:/dev/sdb1

    sudo mount /dev/sdb1 /mnt/pico

load the complied file 

    sudo cp blink.uf2 /mnt/pico

once you pushed code into pico, it automatically umounts and starts running the uploded code





    

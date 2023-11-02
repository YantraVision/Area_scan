
**Raspberry pi user name**: 192.168.90.72

**password**: pi

go to pico-examples and choose appropriate example directory you need to work 

    cd /home/rpf/pico-setup/pico/pico-examples/

    eg: vim /pico/pico-examples/blink/blink.c

once your code is completed
go to build and appropriate example directory to get complie the code you generated, 
        
    cd /home/rpf/pico-setup/pico/pico-examples/build/blink

run 

    make -j4

check dmesg, find which port pico is connected

    sudo /dev/sdb1 /mnt/pico

    sudo cp blink.uf2 /mnt/pico





    

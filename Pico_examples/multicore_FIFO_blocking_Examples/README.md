#Multicore Examples with FIFO Blocking API
This Directory contains four examples which demonstrates the usage of 2 cores with push and pop FIFO buffers which holds upcoming process
	simple_testcase - performs simple comparsion of data feed in both cores.
	passing_functionPointer - Data read/write and processing happens in 2 different cores and the function pointer was pushed as a parameter.
	non-blocking_FIFO_example -  monitors the data entry and perform push/pop operation.
	FIFO_interrupts - Sets Interrupt handler , monitors the data entry and perform push/pop operation.
	queue_handle - Used queues to handle Data parsing.

##APIs Used
###multicore_launch_core1(core1_entry)
	- launches the entry function for core1
###multicore_fifo_push_blocking(FLAG_VALUE)
	- pushes data on to the FIFO and will block until there is space for the data to be sent
###multicore_fifo_pop_blocking()
	- pops data from the FIFO in int32 format and will block until there is data ready to be read.
###multicore_fifo_rvalid()
	- checks the read FIFO buffer to see if there is data waiting and returns true if the FIFO has data.
###multicore_fifo_wready()
	-  Check the write FIFO to see if it has space for more data

#Reference
[multicore_docs](https://www.raspberrypi.com/documentation//pico-sdk/multicore_8h.html)

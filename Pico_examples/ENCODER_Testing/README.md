# Encoder Testing
	It have 4 source files to read encoder pulses and set camera trigger for capture at expected portion,
```
	* arduino_read.c - it reads and check data in loops , state change were decided by current and next state and unable to achieve results beyond average speed.
	* BitwiseCondition.c - flatten the previous code using bitwise operator and able to observe results for maximum speed (single encoder pulse width - 20us)
	* code_review.c - do have debounce condition with state change decided by current and next state
	* timeCheck.c - same logic in BitwiseCondition file with added timestamps.
```

# COMMANDS
```
make -j4
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program ./Signal_read_FIFO.elf verify reset exit"
```

If needed to run `BitwiseCondition` file, replace the file name in CMakeLists.txt file as below.
```
add_executable(Signal_read_FIFO
		timeCheck.c
		)

add_executable(Signal_read_FIFO
		BitwiseCondition.c
		)
```

# TIME CAPTURE
For time estimation, used `time_us_64()` and `get_time()` functions in `timeCheck.c` file and added pulse toggle at the beginning of loop in core1_entry function(observed single execution time - pulse width of ~150 ns) .


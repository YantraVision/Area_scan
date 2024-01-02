# Image Rendering example
Implemented python scripts to write into and read data from shared memory.

## Directory Structure
SharedMemory_example
|___data_write.py
|___data_read.py
|___static
	|___Images
		|___ dummy.png (image read from shared memory)
|___templates
	|___image_render.html
	|___index.html
|____*.jpeg (for local testing)

## Modules used
	* flask - Flask, render_template (webpage rendering)
	* multiprocessing - shared_memory (shared memory accessing)
	* numpy
	* cv2
	* glob

## Command to run
Open two terminals and run both data_write and data_read scripts.
```
python3 data_write.py
python3 data_read.py
```
Visit `http://172.16.0.90:5000`, it'll direct you to homepage.
Visit `http://172.16.0.90:5000/test`, it'll direct you to image rendering page.*Note- host and port can be altered in data_read.py.

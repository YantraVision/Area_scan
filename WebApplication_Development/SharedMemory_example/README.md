# Image Rendering example
Implemented python scripts to write into and read data from shared memory.

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
flask --app data_read.py --debug run --host="172.16.0.90" - to specify host or 
python3 data_read.py
```
Visit `http://172.16.0.90:5000`, it'll direct you to homepage.
Visit `http://172.16.0.90:5000/test`, it'll direct you to image rendering page.

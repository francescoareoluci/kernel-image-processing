# C++ Multithreading Kernel image processing

This repository contains a C++ application that can be used to process an image using the kernel convolution. The application can process .png images and the resulting image will be a grayscale png.

## Compile and Linking

In order to build the application, the png++ library (a libpng wrapper) must be installed. To install it using apt:

> sudo apt install libpng++-dev

Makefile is provided in order to compile the source. To compile, from the root directory run:

> make

## Usage

A main controller (main.cpp) has been written to test the developed classes that are used to load images (image.h, images.cpp) and to apply a kernel to them (kernel.h, kernel.cpp). The main file will load image from requested image path and will write the output image in the output/ folder. It run the kernel processing on the loaded image two times: the first time it will run a parallel processing with the specified number of threads, the second time it will run a sequential processing. Execution times for the two runs will be printed on the command line.
To launch the application:

**Usage: ./kernel_convolution filter_type image_path threads_number** <br>
	**filter_type**: <gaussian | sharpen | edge_detect | laplacian | gaussian_laplacian> <br>
 	**image_path**: specify the image path<br>
 	**threads_number** (optional): number of threads for the parallel run. Default: 4


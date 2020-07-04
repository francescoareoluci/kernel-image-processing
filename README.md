# C++ Multithreading Kernel image processing

This repository contains a C++ application that can be used to process an image using Kernel convolution methods. The application can process .png images and the resulting image will be a grayscale png.

## Compile and Linking

In order to build the application, the png++ library (a libpng wrapper) must be installed. To install it using apt:

> sudo apt install libpng++-dev

To compile the application:

> g++ -c main.cpp kernel.cpp image.cpp `libpng-config --cflags` -pthread

To link:

> g++ -o convolutionKernel main.o kernel.o image.o `libpng-config --ldflags` -pthread

## Usage

A main controller (main.cpp) has been written to test the developed classes that are used to load images (image.h, images.cpp) and to apply a kernel to them (kernel.h, kernel.cpp). The main file will load images from images/ folder, using images enumeration and will write output images in output/ folder. It will do the kernel processing on the loaded images two times: the first time it will run a parallel processing with the specified number of threads, the second time it will run a non-parallel processing. Execution times for the two runs will be printed on the command line.
To launch the main application:

Usage: ./kernelConvolution filter_type threads_number images_numbers
	filter_type: <gaussian | sharpen | edge_detect | alt_edge_detect>
	(optional) threads_number: number of threads for the parallel run. Default: 4
	(optional) images_number: number of images to be loaded from images/ folder. Default: 1

Note: for sake of simplicity images in images/ folder must be enumerated (e.g: 1.png, 2.png, etc).


#include <iostream>
#include "image.h"
#include <chrono>

#define GAUSSIAN_FILTER_COMMAND         "gaussian";
#define SHARPENING_FILTER_COMMAND       "sharpen";
#define EDGE_DETECTION_FILTER_COMMAND   "edge_detect";

enum class FilterType
{
    GAUSSIAN_FILTER,
    SHARPEN_FILTER,
    EDGE_DETECTION,
    ALT_EDGE_DETECTION
};

int main(int argc, char *argv[]) 
{
    std::cout << "===== Multithread kernel convolution =====" << std::endl;

    // Check command line parameters
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " image_path filter_type" << std::endl;
        std::cerr << "filter_type: <gaussian | sharpen | edge_detect | alt_edge_detect>" << std::endl;
        return 1;
    }

    FilterType filterType;
    if (std::string(argv[2]) == "gaussian") {
        filterType = FilterType::GAUSSIAN_FILTER;
    }
    else if (std::string(argv[2]) == "sharpen") {
        filterType = FilterType::SHARPEN_FILTER;
    }
    else if (std::string(argv[2]) == "edge_detect") {
        filterType = FilterType::EDGE_DETECTION;
    }
    else if (std::string(argv[2]) == "alt_edge_detect") {
        filterType = FilterType::ALT_EDGE_DETECTION;
    }
    else {
        std::cerr << "Invalid filter type " << argv[2] << std::endl;
        std::cerr << "filter_type: <gaussian | sharpen | edge_detect | alt_edge_detect>" << std::endl;
        return 1;
    }

    Image* image1 = new Image();
    Image* image2 = new Image();
    Image* image3 = new Image();
    Image* image4 = new Image();
    Image* image5 = new Image();
    Image* image6 = new Image();
    Image* image7 = new Image();
    Image* image8 = new Image();

    int threadNumber = 8;
    
    //image.loadImage(argv[1]);
    std::vector<Image*> images;
    
        image1->loadImage("images/1.png");    
        images.push_back(image1);
        image2->loadImage("images/2.png");    
        images.push_back(image2);
        image3->loadImage("images/3.png");    
        images.push_back(image3);
        image4->loadImage("images/4.png");    
        images.push_back(image4);
        image5->loadImage("images/5.png");    
        images.push_back(image5);
        image6->loadImage("images/6.png");    
        images.push_back(image6);
        image7->loadImage("images/7.png");    
        images.push_back(image7);
        image8->loadImage("images/8.png");    
        images.push_back(image8);

    Kernel filter = Kernel();
    switch (filterType)
    {
        case FilterType::GAUSSIAN_FILTER:
            filter.setGaussianFilter(55, 55, 2);
            break;

        case FilterType::SHARPEN_FILTER:
            filter.setSharpenFilter(3, 3, 5, -1);
            break;

        case FilterType::EDGE_DETECTION:
            filter.setEdgeDetectionFilter(3, 3, 8, -1);
            break;

        case FilterType::ALT_EDGE_DETECTION:
            filter.setAltEdgeDetectionFilter(3, 3, -4, 1);
            break;

        default:
            std::cerr << "Unable to find requested filter, switching to gaussian..." << std::endl;
            filter.setGaussianFilter(5, 5, 2);
            break;
    }
    filter.printKernel();

    Image resultingImage; 
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 8; i++)
    {
        images[0]->multithreadFiltering(resultingImage, filter, threadNumber);
        //images[0]->applyFilter(resultingImage, filter);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto t3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 8; i++)
    {
        images[0]->applyFilter(resultingImage, filter);
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    auto multithreadDuration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    auto singleDuration = std::chrono::duration_cast<std::chrono::microseconds>( t4 - t3 ).count();

    std::cout << "Multithread Execution time: " << multithreadDuration
              << " with threads: " << threadNumber << std::endl;

    std::cout << "Single thread Execution time: " << singleDuration << std::endl;

    resultingImage.saveImage("result.png");

    return 0;    
}
#include <iostream>
#include "image.h"
#include <chrono>

#define GAUSSIAN_FILTER_COMMAND             "gaussian"
#define SHARPENING_FILTER_COMMAND           "sharpen"
#define EDGE_DETECTION_FILTER_COMMAND       "edge_detect"
#define ALT_EDGE_DETECTION_FILTER_COMMAND   "alt_edge_detect"

#define SOURCE_FOLDER   "images/"
#define IMAGE_EXT       ".png"
#define IMAGES_NUMBER   16
#define THREAD_NUMBER   16

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
    if (std::string(argv[2]) == GAUSSIAN_FILTER_COMMAND) {
        filterType = FilterType::GAUSSIAN_FILTER;
    }
    else if (std::string(argv[2]) == SHARPENING_FILTER_COMMAND) {
        filterType = FilterType::SHARPEN_FILTER;
    }
    else if (std::string(argv[2]) == EDGE_DETECTION_FILTER_COMMAND) {
        filterType = FilterType::EDGE_DETECTION;
    }
    else if (std::string(argv[2]) == ALT_EDGE_DETECTION_FILTER_COMMAND) {
        filterType = FilterType::ALT_EDGE_DETECTION;
    }
    else {
        std::cerr << "Invalid filter type " << argv[2] << std::endl;
        std::cerr << "filter_type: <gaussian | sharpen | edge_detect | alt_edge_detect>" << std::endl;
        return 1;
    }

    std::vector<Image*> images;
    for (int i = 1; i < IMAGES_NUMBER + 1; i++) {
        images.push_back(new Image());
        images[i - 1]->loadImage(std::string(std::string(SOURCE_FOLDER) + 
                                            std::to_string(i) + 
                                            std::string(IMAGE_EXT)).c_str());
    }

    std::cout << images.size() << std::endl;
    for (int i = 0; i < images.size(); i++) {
        std::cout << images[i]->getImageWidth() << std::endl;
    }

    Kernel filter = Kernel();
    switch (filterType)
    {
        case FilterType::GAUSSIAN_FILTER:
            filter.setGaussianFilter(25, 25, 2);
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

    // Executing multithread filtering for each image
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < IMAGES_NUMBER; i++) {
        images[i]->multithreadFiltering(resultingImage, filter, THREAD_NUMBER);
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    // Executing non-parallel filtering for each image
    auto t3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < IMAGES_NUMBER; i++) {
        images[i]->applyFilter(resultingImage, filter);
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    
    auto multithreadDuration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    auto singleDuration = std::chrono::duration_cast<std::chrono::microseconds>( t4 - t3 ).count();

    std::cout << "Multithread Execution time: " << multithreadDuration
              << " with threads: " << THREAD_NUMBER << std::endl;

    std::cout << "Single thread Execution time: " << singleDuration << std::endl;

    resultingImage.saveImage("result.png");

    return 0;    
}
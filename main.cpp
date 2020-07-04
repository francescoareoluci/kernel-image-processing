#include <iostream>
#include <chrono>
#include "image.h"


#define GAUSSIAN_FILTER_COMMAND             "gaussian"
#define SHARPENING_FILTER_COMMAND           "sharpen"
#define EDGE_DETECTION_FILTER_COMMAND       "edge_detect"
#define LAPLACIAN_FILTER_COMMAND            "laplacian"
#define GAUSSIAN_LAPLACIAN_COMMAND          "gaussian_laplacian"

#define SOURCE_FOLDER   "images/"
#define OUTPUT_FOLDER   "output/"
#define IMAGE_EXT       ".png"
#define IMAGES_NUMBER   1
#define THREAD_NUMBER   4

enum class FilterType
{
    GAUSSIAN_FILTER,
    SHARPEN_FILTER,
    EDGE_DETECTION,
    LAPLACIAN_FILTER,
    GAUSSIAN_LAPLACIAN_FILTER
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
    else if (std::string(argv[2]) == LAPLACIAN_FILTER_COMMAND) {
        filterType = FilterType::LAPLACIAN_FILTER;
    }
    else if (std::string(argv[2]) == GAUSSIAN_LAPLACIAN_COMMAND) {
        filterType = FilterType::GAUSSIAN_LAPLACIAN_FILTER;
    }
    else {
        std::cerr << "Invalid filter type " << argv[2] << std::endl;
        std::cerr << "filter_type: <gaussian | sharpen | edge_detect | laplacian | gaussian_laplacian >" << std::endl;
        return 1;
    }

    Kernel filter = Kernel();
    switch (filterType)
    {
        case FilterType::GAUSSIAN_FILTER:
            filter.setGaussianFilter(105, 105, 1);
            break;

        case FilterType::SHARPEN_FILTER:
            filter.setSharpenFilter();
            break;

        case FilterType::EDGE_DETECTION:
            filter.setEdgeDetectionFilter();
            break;

        case FilterType::LAPLACIAN_FILTER:
            filter.setLaplacianFilter();
            break;

        case FilterType::GAUSSIAN_LAPLACIAN_FILTER:
            filter.setGaussianLaplacianFilter();
            break;

        default:
            std::cerr << "Unable to find requested filter, switching to gaussian..." << std::endl;
            filter.setGaussianFilter(5, 5, 2);
            break;
    }
    filter.printKernel();

    // Getting images from source folder
    std::vector<Image*> images;
    for (int i = 1; i < IMAGES_NUMBER + 1; i++) {
        images.push_back(new Image());
        images[i - 1]->loadImage(std::string(std::string(SOURCE_FOLDER) + 
                                            std::to_string(i) + 
                                            std::string(IMAGE_EXT)).c_str());
    }
    
    std::vector<Image*> resultingMTImages;
    std::vector<Image*> resultingNPImages;

    // Executing multithread filtering for each image
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < IMAGES_NUMBER; i++) {
        resultingMTImages.push_back(new Image());
        images[i]->multithreadFiltering(*resultingMTImages[i], filter, THREAD_NUMBER);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    
    // Executing non-parallel filtering for each image
    auto t3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < IMAGES_NUMBER; i++) {
        resultingNPImages.push_back(new Image());
        images[i]->applyFilter(*resultingNPImages[i], filter);
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    // Evaluating execution times
    auto multithreadDuration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    auto singleDuration = std::chrono::duration_cast<std::chrono::microseconds>( t4 - t3 ).count();

    std::cout << "Multithread Execution time: " << multithreadDuration
              << " with threads: " << THREAD_NUMBER << std::endl;

    std::cout << "Single thread Execution time: " << singleDuration << std::endl;

    // Saving resulting images
    for (int i = 0; i < resultingMTImages.size(); i++) {
        resultingMTImages[i]->saveImage(std::string(std::string(OUTPUT_FOLDER) + 
                                        std::to_string(i + 1) + "_" + std::string(argv[2]) +
                                        std::string(IMAGE_EXT)).c_str());
    }

    for (int i = 0; i < images.size(); i++) {
        delete images[i];
        delete resultingMTImages[i];
        delete resultingNPImages[i];
    } 

    images.clear();
    resultingMTImages.clear();
    resultingNPImages.clear();

    return 0;    
}
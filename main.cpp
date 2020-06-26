#include <iostream>
#include "image.h"

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

    Image image;
    image.loadImage(argv[1]);

    Kernel filter = Kernel();
    switch (filterType)
    {
        case FilterType::GAUSSIAN_FILTER:
            filter.setGaussianFilter(5, 5, 2);
            break;

        case FilterType::SHARPEN_FILTER:
            filter.setSharpenFilter(3, 3, 5, -1);
            break;

        case FilterType::EDGE_DETECTION:
            filter.setEdgeDetectionFilter(3, 3, -4, 1);
            break;

        case FilterType::ALT_EDGE_DETECTION:
            filter.setAltEdgeDetectionFilter(3, 3, 8, -1);

        default:
            std::cerr << "Unable to find requested filter, switching to gaussian..." << std::endl;
            filter.setGaussianFilter(5, 5, 2);
            break;
    }
    filter.printKernel();

    Image resultingImage; 
    image.applyFilter(resultingImage, filter);

    resultingImage.saveImage("result.png");

    return 0;    
}
#include <png++/png.hpp>
#include <vector>
#include <assert.h>
#include <cmath>

#include "kernel.h"

typedef std::vector<double> Array;
typedef std::vector<Array> Matrix;
// To handle multiple channel images
typedef std::vector<Matrix> Image;

#define GAUSSIAN_FILTER_COMMAND         "gaussian";
#define SHARPENING_FILTER_COMMAND       "sharpen";
#define EDGE_DETECTION_FILTER_COMMAND   "edge_detect";

enum class FilterType
{
    GAUSSIAN_FILTER,
    SHARPEN_FILTER,
    EDGE_DETECTION
};

void printKernel(const Matrix &filter) 
{
    int height = filter.size();
    int width = filter[0].size();

    std::cout << std::endl;
    std::cout << "=== Kernel ===" << std::endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << filter[i][j] << " ";
        }   
        std::cout << "" << std::endl; 
    }
    std::cout << "==============" << std::endl;
    std::cout << std::endl;
}

Matrix getGaussianFilter(int height, int width, double sigma)
{
    std::cout << "Building gaussian filter..." << std::endl;

    Matrix kernel(height, Array(width));
    double sum = 0.0;

    if (height != width || height % 2 == 0 || width % 2 == 0) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        height = 3;
        width = 3;
    }

    int middleHeight = int(height / 2);
    int middleWidth = int(width / 2);

    for (int i = -middleHeight; i <= middleHeight; i++) {
        for (int j = -middleWidth; j <= middleWidth; j++) {
            kernel[i + middleHeight][j + middleWidth] = exp(- (i * i + j * j) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            sum += kernel[i + middleHeight][j + middleWidth];
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            kernel[i][j] /= sum;
        }
    }

    printKernel(kernel);

    return kernel;
}

Matrix getSharpenFilter(int height, int width, int amp, int deamp) 
{
    std::cout << "Building sharpening filter..." << std::endl;

    Matrix kernel(height, Array(width));

    if (height != width || height % 2 == 0 || width % 2 == 0) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        height = 3;
        width = 3;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if ((i == int(height / 2)) && (j == int(width / 2))) {
                kernel[i][j] = amp;
            }
            else {
                kernel[i][j] = -deamp;
            }
        }
    }

    kernel[0][0] = 0;
    kernel[0][width -1] = 0;
    kernel[height -1][0] = 0;
    kernel[height -1][width -1] = 0;

    printKernel(kernel);

    return kernel;
}

Matrix getEdgeDetectionFilter(int height, int width, int amp, int deamp) 
{
    std::cout << "Building edge detector filter..." << std::endl;

    Matrix kernel(height, Array(width));

    if (height != width || height % 2 == 0 || width % 2 == 0) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        height = 3;
        width = 3;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if ((i == int(height / 2)) && (j == int(width / 2))) {
                kernel[i][j] = amp;
            }
            else {
                kernel[i][j] = -deamp;
            }
        }
    }

    printKernel(kernel);

    return kernel;
}

Matrix getEdgeDetectionFilter2(int height, int width, int amp, int deamp) 
{
    std::cout << "Building edge detector filter..." << std::endl;

    Matrix kernel(height, Array(width));

    if (height != width || height % 2 == 0 || width % 2 == 0) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        height = 3;
        width = 3;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if ((i == int(height / 2)) && (j == int(width / 2))) {
                kernel[i][j] = amp;
            }
            else {
                kernel[i][j] = -deamp;
            }
        }
    }

    kernel[0][0] = 0;
    kernel[0][width -1] = 0;
    kernel[height -1][0] = 0;
    kernel[height -1][width -1] = 0;

    printKernel(kernel);

    return kernel;
}

Image loadImage(const char *filename)
{
    // Load image
    png::image<png::gray_pixel> image(filename);

    // Build matrix from image
    Image imageMatrix(3, Matrix(image.get_height(), Array(image.get_width())));
    
    for (int h = 0; h < image.get_height(); h++) {
        for (int w = 0; w < image.get_width(); w++) {
            imageMatrix[0][h][w] = image[h][w];
            //imageMatrix[1][h][w] = image[h][w].green;
            //imageMatrix[2][h][w] = image[h][w].blue;
        }
    }

    return imageMatrix;
}

void saveImage(const Image &image, const char *filename)
{
    assert(image.size() == 3);

    int height = image[0].size();
    int width = image[0][0].size();

    png::image<png::gray_pixel> imageFile(width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            imageFile[y][x] = image[0][y][x];
            //imageFile[y][x].green = image[1][y][x];
            //imageFile[y][x].blue = image[2][y][x];
        }
    }
    imageFile.write(filename);

    std::cout << "Image saved in " << std::string(filename) << std::endl;
}

Image applyFilter(const Image &image, const Matrix &filter)
{
    std::cout << "Applying filter to image" << std::endl;

    // Checking image channels and kernel size
    assert(image.size() == 3 && filter.size() != 0);

    // Get image dimensions
    int channels = image.size();
    int height = image[0].size();
    int width = image[0][0].size();

    // Get filter dimensions
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();

    // TODO: input padding w.r.t. filter size
    Image paddedImage(channels, Matrix(height + filterHeight - 1, Array(width + filterWidth - 1))); 
    for (int h = 0; h < paddedImage[0].size(); h++) {
        for (int w = 0; w < paddedImage[0][0].size(); w++) {
            if ((h < int(filterHeight / 2) + 1) || (w < int(filterWidth / 2) + 1) || 
                (h > height - int(filterHeight / 4) - 1) || (w > width - int(filterWidth / 4) - 1)) {
                paddedImage[0][h][w] = 0.0;
                paddedImage[1][h][w] = 0.0;
                paddedImage[2][h][w] = 0.0;
            }
            else {
                paddedImage[0][h][w] = image[0][h][w];
                paddedImage[1][h][w] = image[1][h][w];
                paddedImage[2][h][w] = image[2][h][w];
            }
        }
    }

    // Build final image dimensions
    int newImageHeight = height;
    int newImageWidth = width;

    Image newImage(channels, Matrix(newImageHeight, Array(newImageWidth)));

    for (int d = 0; d < channels; d++) {
        for (int i = 0; i < newImageHeight; i++) {
            for (int j = 0; j < newImageWidth; j++) {
                for (int h = i;  h < i + filterHeight; h++) {
                    for (int w = j; w < j + filterWidth; w++) {
                        newImage[d][i][j] += filter[h - i][w - j] * paddedImage[d][h][w];
                    }
                }
            }
        }
    }

    std::cout << "Done!" << std::endl;

    return newImage;
}

Image applyFilter(Image &image, Matrix &filter, int times)
{
    Image newImage = image;
    for(int i = 0; i < times; i++) {
        newImage = applyFilter(newImage, filter);
    }
    return newImage;
}

int main(int argc, char *argv[]) 
{
    std::cout << "===== Multithread kernel convolution =====" << std::endl;

    // Check command line parameters
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " image_path filter_type" << std::endl;
        std::cerr << "filter_type: <gaussian | sharpen | edge_detect>" << std::endl;
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
    else {
        std::cerr << "Invalid filter type " << argv[2] << std::endl;
        std::cerr << "filter_type: <gaussian>" << std::endl;
        return 1;
    }

    Image img = loadImage(argv[1]);

    Matrix filter;
    switch (filterType)
    {
        case FilterType::GAUSSIAN_FILTER:
            filter = getGaussianFilter(5, 5, 2);
            break;

        case FilterType::SHARPEN_FILTER:
            filter = getSharpenFilter(3, 3, 5, 1);
            break;

        case FilterType::EDGE_DETECTION:
            filter = getEdgeDetectionFilter2(3, 3, 4, 1);
            break;

        default:
            std::cerr << "Unable to find requested filter, switching to gaussian..." << std::endl;
            filter = getGaussianFilter(5, 5, 5);
            break;
    }

    Image finalImage = applyFilter(img, filter, 1);

    saveImage(finalImage, "result.png");

    return 0;    
}
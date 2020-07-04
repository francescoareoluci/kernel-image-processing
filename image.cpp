#include <png++/png.hpp>
#include <math.h>
#include "image.h"


void threadConv(const double* sourceImage, 
                int startLine, int stopLine,
                std::vector<double>& outImage, 
                const double* mask,
                int width, int height, int channels, 
                int filterWidth, int filterHeight);

Image::Image()
{
    m_threads = std::vector<std::thread>();
}

int Image::getImageWidth() const
{
    return m_imageWidth;
}

int Image::getImageHeight() const
{
    return m_imageHeight;
}

int Image::getImageChannels() const
{
    //return m_image.size();
    return 1;
}

bool Image::setImage(const std::vector<double>& source, int width, int height)
{
    this->m_image = source;
    this->m_imageWidth = width;
    this->m_imageHeight = height;

    return true;
}

std::vector<double> Image::getImage() const
{
    return this->m_image;
}

bool Image::loadImage(const char *filename)
{
    // Load image
    png::image<png::gray_pixel> image(filename);

    // Build matrix from image    
    m_imageHeight = image.get_height();
    m_imageWidth = image.get_width();
    std::vector<double> imageMatrix(m_imageHeight * m_imageWidth);
    
    for (int h = 0; h < image.get_height(); h++) {
        for (int w = 0; w < image.get_width(); w++) {
            imageMatrix[w + h * m_imageWidth] = image[h][w];
            //imageMatrix[1][h][w] = image[h][w].green;
            //imageMatrix[2][h][w] = image[h][w].blue;
        }
    }

    m_image = imageMatrix;

    return true;
}

bool Image::saveImage(const char *filename) const
{
    int height = this->getImageHeight();
    int width = this->getImageWidth();

    png::image<png::gray_pixel> imageFile(width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            imageFile[y][x] = m_image[x + y * width];
            //imageFile[y][x].green = m_image[1][y][x];
            //imageFile[y][x].blue = m_image[2][y][x];
        }
    }
    imageFile.write(filename);

    std::cout << "Image saved in " << std::string(filename) << std::endl;

    return true;
}

bool Image::applyFilter(Image& resultingImage, const Kernel& kernel) const
{
    std::cout << "Applying filter to image" << std::endl;

    std::vector<double> newImage = applyFilterCommon(kernel);

    resultingImage.setImage(newImage, m_imageWidth, m_imageHeight);
    std::cout << "Done!" << std::endl;

    newImage.clear();

    return true;
}

bool Image::applyFilter(const Kernel& kernel)
{
    std::cout << "Applying filter to image" << std::endl;
    
    std::vector<double> newImage = applyFilterCommon(kernel);
    if (newImage.empty()) {
        return false;
    }

    this->setImage(newImage, m_imageWidth, m_imageHeight);

    std::cout << "Done!" << std::endl;

    newImage.clear();

    return true;
}

std::vector<double> Image::applyFilterCommon(const Kernel& kernel) const
{
    // Get image dimensions
    int channels = this->getImageChannels();
    int height = this->getImageHeight();
    int width = this->getImageWidth();

    // Get filter dimensions
    int filterHeight = kernel.getKernelHeight();
    int filterWidth = kernel.getKernelWidth();

    // Checking image channels and kernel size
    if (channels != 1) {
        std::cerr << "Invalid number of image's channels" << std::endl;
        return std::vector<double>();
    }

    if (filterHeight == 0 || filterWidth == 0) {
        std::cerr << "Invalid filter dimension" << std::endl;
        return std::vector<double>();
    }

    // Input padding w.r.t. filter size
    std::vector<double> paddedImage = buildReplicatePaddedImage(floor(filterHeight/2), floor(filterWidth/2));
    std::vector<double> newImage(height * width);

    // Get kernel matrix
    std::vector<double> mask = kernel.getKernel();

    // Use pointers to speed up pixels access
    const double* maskPtr = {mask.data()};
    const double* paddedImagePtr = {paddedImage.data()};

    int paddedWidth = width + floor(filterWidth / 2) * 2;
    int pixelSum = 0;

    int filterRowIndex = 0;
    int sourceImgRowIndex = 0;
    int sourceImgLineIndex = 0;
    int outImgRowIndex = 0;


    // Apply convolution
    for (int d = 0; d < channels; d++) {
        for (int i = 0; i < height; i++) {
            outImgRowIndex = i * width;
            for (int j = 0; j < width; j++) {
                for (int h = 0;  h < filterHeight; h++) {
                    filterRowIndex = h * filterWidth;
                    sourceImgRowIndex = (h + i) * paddedWidth;
                    for (int w = 0; w < filterWidth; w++) {
                        pixelSum += maskPtr[w + filterRowIndex] * 
                                        paddedImagePtr[w + j + sourceImgRowIndex];
                    }
                }
                if (pixelSum < 0) {
                    pixelSum = 0;
                }
                else if (pixelSum > 255) {
                    pixelSum = 255;
                }
                newImage[j + outImgRowIndex] = pixelSum;
                pixelSum = 0;
            }
        }
    }
    paddedImage.clear();
    mask.clear();
    
    return newImage;
}

bool Image::multithreadFiltering(Image& resultingImage, const Kernel& kernel, int threadsNumber)
{
    std::cout << "Applying multithread filter to image" << std::endl;

    // Get image dimensions
    int channels = this->getImageChannels();
    int height = this->getImageHeight();
    int width = this->getImageWidth();

    // Get filter dimensions
    int filterHeight = kernel.getKernelHeight();
    int filterWidth = kernel.getKernelWidth();

    // Input padding w.r.t. filter size
    std::vector<double> paddedImage = buildReplicatePaddedImage(floor(filterHeight/2), floor(filterWidth/2));
    std::vector<double> newImage(height * width);

    // Get kernel matrix
    std::vector<double> mask = kernel.getKernel();

    // Use pointers to speed up pixels access
    const double* maskPtr = {mask.data()};
    const double* paddedImagePtr = {paddedImage.data()};
    
    int startLine = 0;
    int stopLine = 0;

    for (int i = 0; i < threadsNumber; i++) {
        // If more thread than images row are requested
        if (i > height) {
            break;
        }

        // Evaluate start line and stop line for each thread
        if (i == 0) {
            startLine = 0;
        }
        else { 
            startLine = stopLine;
        }
        stopLine = floor(height / threadsNumber) * (i + 1);

        // Check if the last thread will iterate 
        // until the end of the matrix
        if (i == threadsNumber - 1) {
            if (stopLine != height) {
                stopLine = height;
            }
        }

        // Create threads and assign to them 
        // the threadConv function
        m_threads.push_back(std::thread(threadConv, paddedImagePtr, 
                                startLine, stopLine, std::ref(newImage), maskPtr, 
                                width, height, channels, filterWidth, filterHeight));          
    }

    // Once joined, threads will be removed from the vector
    for (int i = 0; i < m_threads.size(); i++) {
        m_threads[i].join();
    }

    resultingImage.setImage(newImage, m_imageWidth, m_imageHeight);

    std::cout << "Done!" << std::endl;

    paddedImage.clear();
    newImage.clear();
    mask.clear();
    
    return true;
}

void threadConv(const double* sourceImage, 
                int startLine, int stopLine, 
                std::vector<double>& outImage, 
                const double* mask,
                int width, int height, int channels, 
                int filterWidth, int filterHeight)
{
    int paddedWidth = width + floor(filterWidth / 2) * 2;
    int pixelSum = 0;

    int filterRowIndex = 0;
    int sourceImgRowIndex = 0;
    int outImgRowIndex = 0;

    // Apply convolution
    for (int d = 0; d < channels; d++) {
        for (int l = startLine; l < stopLine; l++) {
            outImgRowIndex = l * width;
            for (int j = 0; j < width; j++) {
                for (int h = 0; h < filterHeight; h++) {
                    filterRowIndex = h * filterWidth;
                    sourceImgRowIndex = (h + l) * paddedWidth;
                    for (int w = 0; w < filterWidth; w++) {
                        pixelSum += mask[w + filterRowIndex] * 
                                        sourceImage[w + j + sourceImgRowIndex];
                    }
                }
                if (pixelSum < 0) {
                    pixelSum = 0;
                }
                else if (pixelSum > 255) {
                    pixelSum = 255;
                }
                outImage[j + outImgRowIndex] = pixelSum;
                pixelSum = 0;
            }
        }
    }
}

std::vector<double> Image::buildReplicatePaddedImage(const int paddingHeight,
                                                    const int paddingWidth) const
{
    int height = this->getImageHeight();
    int width = this->getImageWidth();

    int paddedHeight = height + paddingHeight * 2;
    int paddedWidth = width + paddingWidth * 2;
    int maxHImageBoundary = height - 1;
    int maxWImageBoundary = width - 1;
    int paddedImageRowIndex = 0;

    std::vector<double> paddedImage(paddedHeight * paddedWidth);
    std::vector<double> sourceImage = this->m_image;

    for (int h = 0; h < paddedHeight; h++) {
        paddedImageRowIndex = h * paddedWidth;
        for (int w = 0; w < paddedWidth; w++) {
            if ((h < paddingHeight) && (w < paddingWidth)) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[0];
            }
            else if ((h > maxHImageBoundary) && (w > maxWImageBoundary)) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(width - 1) + (height - 1) * width];
            }
            else if ((h < paddingHeight) && (w > maxWImageBoundary)) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(width - 1) + (0) * width];
            }
            else if ((w < paddingWidth) && (h > maxHImageBoundary)) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(0) + (height - 1) * width];
            }
            else if (h < paddingHeight) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(w) + (0) * width];
            }
            else if (w < paddingWidth) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(0) + (h) * width];
            }
            else if (h > maxHImageBoundary) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(w) + (height - 1) * width];
            }
            else if (w > maxWImageBoundary) {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(width - 1) + (h) * width];
            }
            else {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(w - paddingWidth) + (h - paddingHeight) * width];
            }
        }
    }

    sourceImage.clear();

    //Image pImg;
    //pImg.setImage(paddedImage, paddedWidth, paddedHeight);
    //pImg.saveImage("padded.png");

    return paddedImage;
}

std::vector<double> Image::buildZeroPaddingImage(const int paddingHeight,
                                                const int paddingWidth) const
{
    int height = this->getImageHeight();
    int width = this->getImageWidth();

    int paddedHeight = height + paddingHeight * 2;
    int paddedWidth = width + paddingWidth * 2;
    int maxHImageBoundary = height + paddingHeight - 1;
    int maxWImageBoundary = width + paddingWidth - 1;
    int paddedImageRowIndex = 0;

    std::vector<double> paddedImage(paddedHeight * paddedWidth);
    std::vector<double> sourceImage = this->m_image;

    for (int h = 0; h < paddedHeight; h++) {
        paddedImageRowIndex = h * paddedWidth;
        for (int w = 0; w < paddedWidth; w++) {
            if ((h < paddingHeight) || (w < paddingWidth) || 
                (h > maxHImageBoundary) || 
                (w > maxWImageBoundary)) {
                paddedImage[w + paddedImageRowIndex] = 0.0;
            }
            else {
                paddedImage[w + paddedImageRowIndex] = sourceImage[(w - paddingWidth) + (h - paddingHeight) * width];
            }
        }
    }

    sourceImage.clear();

    //Image pImg;
    //pImg.setImage(paddedImage, paddedWidth, paddedHeight);
    //pImg.saveImage("padded.png");

    return paddedImage;
}
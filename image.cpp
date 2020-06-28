#include <png++/png.hpp>
#include "image.h"


void threadConv(const std::vector<double>& sourceImage, 
                int startLine, int stopLine,
                std::vector<double>& outImage, 
                const std::vector<double>& mask,
                int width, int height, int channels, int filterWidth, int filterHeight,
                int threadsNumber);

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
    //if (m_image.size() != 1)
    //{
    //    std::cerr << "Unable to save image" << std::endl;
    //    return false;
    //}

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
    std::vector<double> paddedImage = buildPaddedImage(filterHeight, filterWidth);
    std::vector<double> newImage(height * width);

    std::vector<double> mask = kernel.getKernel();

    int hOffset = int(filterHeight / 2) + 1;
    int wOffset = int(filterWidth / 2) + 1;
    int paddedWidth = width + (filterWidth * 2) - 1;

    // Apply convolution
    for (int d = 0; d < channels; d++) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                for (int h = i;  h < i + filterHeight; h++) {
                    for (int w = j; w < j + filterWidth; w++) {
                        newImage[j + i * width] += mask[(w - j) + (h - i) * filterWidth] * 
                                                    paddedImage[(w + wOffset) + (h + hOffset) * paddedWidth];
                    }
                }
            }
        }
    }
    paddedImage.clear();
    
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
    std::vector<double> paddedImage = buildPaddedImage(filterHeight, filterWidth);
    std::vector<double> newImage(height * width);

    std::vector<double> mask = kernel.getKernel();
    int startLine = 0;
    int stopLine = 0;

    for (int i = 0; i < threadsNumber; i++) {
        // If more thread than images row are requested
        if (i > height) {
            break;
        }

        if (i == 0) {
            startLine = 0;
        }
        else { 
            startLine = stopLine;
        }
        stopLine = int(height / threadsNumber) * (i + 1);
        // Check if the last thread will iterate 
        // until the end of the matrix
        if (i == threadsNumber - 1) {
            if (stopLine != height) {
                stopLine = height;
            }
        }

        m_threads.push_back(std::thread(threadConv, std::ref(paddedImage), 
                                startLine, stopLine, std::ref(newImage), std::ref(mask), 
                                width, height, channels, filterWidth, filterHeight, threadsNumber));
    }

    // Once joined, threads will be removed from the vector
    for (int i = 0; i < m_threads.size(); i++) {
        m_threads[i].join();
    }

    paddedImage.clear();
    resultingImage.setImage(newImage, m_imageWidth, m_imageHeight);

    std::cout << "Done!" << std::endl;
    
    return true;
}

void threadConv(const std::vector<double>& sourceImage, 
                int startLine, int stopLine, 
                std::vector<double>& outImage, 
                const std::vector<double>& mask,
                int width, int height, int channels, int filterWidth, int filterHeight,
                int threadsNumber)
{
    int hOffset = int(filterHeight / 2) + 1;
    int wOffset = int(filterWidth / 2) + 1;
    int paddedWidth = width + (filterWidth * 2) - 1;

    // Apply convolution
    for (int d = 0; d < channels; d++) {
        for (int l = startLine; l < stopLine; l++) {
            for (int j = 0; j < width; j++) {
                for (int h = l;  h < l + filterHeight; h++) {
                    for (int w = j; w < j + filterWidth; w++) {
                        //outImage[d][l][j] += mask[h - l][w - j] * sourceImage[d][h + hOffset][w + wOffset];
                        outImage[j + l * width] += mask[(w - j) + (h - l) * filterWidth] * 
                                                    sourceImage[(w + wOffset) + (h + hOffset) * paddedWidth];
                    }
                }
            }
        }
    }
}

std::vector<double> Image::buildPaddedImage(const int paddingHeight,
                                            const int paddingWidth) const
{
    int height = this->getImageHeight();
    int width = this->getImageWidth();
    bool first = true;

    int paddedHeight = height + (paddingHeight * 2) - 1;
    int paddedWidth = width + (paddingWidth * 2) - 1;

    std::vector<double> paddedImage(paddedHeight * paddedWidth);

    for (int h = 0; h < paddedHeight; h++) {
        for (int w = 0; w < paddedWidth; w++) {
            if ((h < paddingHeight) || (w < paddingWidth) || 
                (h > height + paddingHeight - 1) || 
                (w > width + paddingWidth - 1)) {
                paddedImage[w + h* paddedWidth] = 0.0;
                //paddedImage[1][h][w] = 0.0;
                //paddedImage[2][h][w] = 0.0;
            }
            else {
                paddedImage[w + h * paddedWidth] = this->m_image[(w - paddingWidth) + (h - paddingHeight) * m_imageWidth];
                //paddedImage[0][h][w] = this->m_image[0][h - paddingHeight][w - paddingWidth];
                //paddedImage[1][h][w] = this->m_image[1][h][w];
                //paddedImage[2][h][w] = this->m_image[2][h][w];
            }
        }
    }

    return paddedImage;
}
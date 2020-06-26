#include "kernel.h"
#include <iostream>
#include <cmath>

Kernel::Kernel() 
{}

void Kernel::printKernel() const
{
    int height = this->getKernelWidth();
    int width = this->getKernelHeight();

    if (height == 0 || width == 0)
    {
        std::cout << "Kernel has not been set up" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== Kernel ===" << std::endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << m_filterMatrix[i][j] << " ";
        }   
        std::cout << "" << std::endl; 
    }
    std::cout << "==============" << std::endl;
    std::cout << std::endl;
}

bool Kernel::setGaussianFilter(const int height, const int width, const double stdDev)
{
    std::cout << "Building gaussian filter..." << std::endl;

    Matrix kernel(height, Array(width));
    double sum = 0.0;

    if (height != width || height % 2 == 0 || width % 2 == 0) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;

        return false;
    }

    if (stdDev <= 0) {
        std::cerr << "Standard deviation value is not valid" << std::endl;
        std::cerr << "Standard deviation value must be positive" << std::endl;

        return false;
    }

    int middleHeight = int(height / 2);
    int middleWidth = int(width / 2);

    for (int i = -middleHeight; i <= middleHeight; i++) {
        for (int j = -middleWidth; j <= middleWidth; j++) {
            kernel[i + middleHeight][j + middleWidth] = exp(- (i * i + j * j) / (2 * stdDev * stdDev)) / 
                                                            (2 * M_PI * stdDev * stdDev);
            sum += kernel[i + middleHeight][j + middleWidth];
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            kernel[i][j] /= sum;
        }
    }

    m_filterMatrix = kernel;
    return true;
}

bool Kernel::setSharpenFilter(int height, int width, int max, int min)
{
    if (height != width || height != 3 || width != 3) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        return false;
    }

    if (min >= 0 || max <= 0) {
        std::cerr << "Max and min value are not valid" << std::endl;
        std::cerr << "Max value must be positive and min value must be negative" << std::endl;
        return false;
    }

    Matrix kernel(height, Array(width));
    this->buildKernelCommon(kernel, max, min);

    kernel[0][0] = 0;
    kernel[0][width -1] = 0;
    kernel[height -1][0] = 0;
    kernel[height -1][width -1] = 0;

    m_filterMatrix = kernel;
    return true;
}

bool Kernel::setEdgeDetectionFilter(int height, int width, int max, int min)
{
    if (height != width || height != 3 || width != 3) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        return false;
    }

    if (min >= 0 || max <= 0) {
        std::cerr << "Max and min value are not valid" << std::endl;
        std::cerr << "Max value must be positive and min value must be negative" << std::endl;
        return false;
    }

    Matrix kernel(height, Array(width));
    this->buildKernelCommon(kernel, max, min);

    m_filterMatrix = kernel;
    return true;
}

bool Kernel::setAltEdgeDetectionFilter(int height, int width, int max, int min)
{
    if (height != width || height != 3 || width != 3) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        return false;
    }

    if (min <= 0 || max >= 0) {
        std::cerr << "Max and min value are not valid" << std::endl;
        std::cerr << "Max value must be negative and min value must be positive" << std::endl;
        return false;
    }

    Matrix kernel(height, Array(width));
    this->buildKernelCommon(kernel, max, min);

    kernel[0][0] = 0;
    kernel[0][width -1] = 0;
    kernel[height -1][0] = 0;
    kernel[height -1][width -1] = 0;

    m_filterMatrix = kernel;
    return true;
}

bool Kernel::buildKernelCommon(Matrix &kernel, int max, int min)
{
    int height = kernel.size();
    int width = kernel[0].size();

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if ((i == int(height / 2)) && (j == int(width / 2))) {
                kernel[i][j] = max;
            }
            else {
                kernel[i][j] = min;
            }
        }
    }

    return true;
}

int Kernel::getKernelWidth() const 
{
    return m_filterMatrix[0].size();
}
        
int Kernel::getKernelHeight() const
{
    return m_filterMatrix.size();
}

Matrix Kernel::getKernel() const
{
    return this->m_filterMatrix;
}
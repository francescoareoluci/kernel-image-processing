#include "kernel.h"
#include <iostream>
#include <cmath>

Kernel::Kernel() 
{}

void Kernel::printKernel() const
{
    int height = m_filterHeight;
    int width = m_filterWidth;

    if (height == 0 || width == 0)
    {
        std::cout << "Kernel has not been set up" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "=== Kernel ===" << std::endl;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::cout << m_filterMatrix[j + i * width] << " ";
        }   
        std::cout << "" << std::endl; 
    }
    std::cout << "==============" << std::endl;
    std::cout << std::endl;
}

bool Kernel::setGaussianFilter(const int height, const int width, const double stdDev)
{
    std::cout << "Building gaussian filter..." << std::endl;

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

    std::vector<double> kernel(width * height);
    double sum = 0.0;

    int middleHeight = int(height / 2);
    int middleWidth = int(width / 2);

    for (int i = -middleHeight; i <= middleHeight; i++) {
        for (int j = -middleWidth; j <= middleWidth; j++) {
            double cellValue = exp(- (i * i + j * j) / (2 * stdDev * stdDev)) / (2 * M_PI * stdDev * stdDev);
            kernel[(j + middleWidth) + ( i + middleHeight) * width] = cellValue;
            sum += cellValue;
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            kernel[j + i * width] /= sum;
        }
    }

    m_filterMatrix = kernel;
    m_filterWidth = width;
    m_filterHeight = height;

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

    std::vector<double> kernel(width * height);
    this->buildKernelCommon(kernel, max, min, height, width);

    kernel[0] = 0.0;
    kernel[width - 1] = 0.0;
    kernel[height - 1] = 0.0;
    kernel[kernel.size() - 1] = 0.0;

    m_filterMatrix = kernel;
    m_filterWidth = width;
    m_filterHeight = height;

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

    std::vector<double> kernel(height * width);
    this->buildKernelCommon(kernel, max, min, height, width);

    m_filterMatrix = kernel;
    m_filterWidth = width;
    m_filterHeight = height;

    return true;
}

bool Kernel::setAltEdgeDetectionFilter(int height, int width, int max, int min)
{
    if (height != width || height != 3 || width != 3) {
        std::cerr << "Height and Width values are not valid" << std::endl;
        std::cerr << "Width and height should have the same values" << std::endl;
        return false;
    }

    //if (min <= 0 || max >= 0) {
    //    std::cerr << "Max and min value are not valid" << std::endl;
    //    std::cerr << "Max value must be negative and min value must be positive" << std::endl;
    //    return false;
    //}

    std::vector<double> kernel(height * width);
    this->buildKernelCommon(kernel, max, min, height, width);

    kernel[0] = 0.0;
    kernel[width - 1] = 0.0;
    kernel[height - 1] = 0.0;
    kernel[kernel.size() - 1] = 0.0;

    m_filterMatrix = kernel;
    m_filterWidth = width;
    m_filterHeight = height;

    return true;
}

bool Kernel::buildKernelCommon(std::vector<double> &kernel, int max, int min, int height, int width)
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if ((i == int(height / 2)) && (j == int(width / 2))) {
                kernel[j + i * width] = max;
            }
            else {
                kernel[j + i * width] = min;
            }
        }
    }

    return true;
}

int Kernel::getKernelWidth() const 
{
    return m_filterWidth;
}
        
int Kernel::getKernelHeight() const
{
    return m_filterHeight;
}

std::vector<double> Kernel::getKernel() const
{
    return this->m_filterMatrix;
}
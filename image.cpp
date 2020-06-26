#include <png++/png.hpp>
#include "image.h"

Image::Image()
{}

int Image::getImageWidth() const
{
    return m_image[0][0].size();
}

int Image::getImageHeight() const
{
    return m_image[0].size();
}

int Image::getImageChannels() const
{
    return m_image.size();
}

bool Image::setImage(const MatrixChannels& source)
{
    this->m_image = source;
}

bool Image::loadImage(const char *filename)
{
    // Load image
    png::image<png::rgb_pixel> image(filename);

    // Build matrix from image
    MatrixChannels imageMatrix(3, Matrix(image.get_height(), Array(image.get_width())));
    
    for (int h = 0; h < image.get_height(); h++) {
        for (int w = 0; w < image.get_width(); w++) {
            imageMatrix[0][h][w] = image[h][w].red;
            imageMatrix[1][h][w] = image[h][w].green;
            imageMatrix[2][h][w] = image[h][w].blue;
        }
    }

    m_image = imageMatrix;

    return true;
}

bool Image::saveImage(const char *filename) const
{
    if (m_image.size() != 3)
    {
        std::cerr << "Unable to save image" << std::endl;
        return false;
    }

    int height = this->getImageHeight();
    int width = this->getImageWidth();

    png::image<png::rgb_pixel> imageFile(width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            imageFile[y][x].red = m_image[0][y][x];
            imageFile[y][x].green = m_image[1][y][x];
            imageFile[y][x].blue = m_image[2][y][x];
        }
    }
    imageFile.write(filename);

    std::cout << "Image saved in " << std::string(filename) << std::endl;

    return true;
}

bool Image::applyFilter(Image& resultingImage, const Kernel& kernel) const
{
    std::cout << "Applying filter to image" << std::endl;

    // Get image dimensions
    int channels = this->getImageChannels();
    int height = this->getImageHeight();
    int width = this->getImageWidth();

    // Get filter dimensions
    int filterHeight = kernel.getKernelHeight();
    int filterWidth = kernel.getKernelWidth();

    // Checking image channels and kernel size
    if (channels != 3) {
        // TODO
        return false;
    }

    if (filterHeight == 0 || filterWidth == 0) {
        // TODO
        return false;
    }

    // TODO: input padding w.r.t. filter size
    MatrixChannels paddedImage = buildPaddedImage(height, width, filterHeight, filterWidth);

    // Build final image dimensions
    int newImageHeight = height;
    int newImageWidth = width;

    MatrixChannels newImage(channels, Matrix(newImageHeight, Array(newImageWidth)));
    Matrix mask = kernel.getKernel();

    for (int d = 0; d < channels; d++) {
        for (int i = 0; i < newImageHeight; i++) {
            for (int j = 0; j < newImageWidth; j++) {
                for (int h = i;  h < i + filterHeight; h++) {
                    for (int w = j; w < j + filterWidth; w++) {
                        newImage[d][i][j] += mask[h - i][w - j] * paddedImage[d][h][w];
                    }
                }
            }
        }
    }

    resultingImage.setImage(newImage);
    std::cout << "Done!" << std::endl;

    return true;
}

MatrixChannels Image::buildPaddedImage(const int startingHeight, 
                                        const int startingWidth,
                                        const int paddingHeight,
                                        const int paddingWidth) const
{
    MatrixChannels paddedImage(3, Matrix(startingHeight + paddingHeight - 1, 
                                        Array(startingWidth + paddingWidth - 1))); 

    for (int h = 0; h < paddedImage[0].size(); h++) {
        for (int w = 0; w < paddedImage[0][0].size(); w++) {
            if ((h < int(paddingHeight / 2) + 1) || (w < int(paddingWidth / 2) + 1) || 
                (h > startingHeight - int(paddingHeight / 4) - 1) || 
                (w > startingWidth - int(paddingWidth / 4) - 1)) {
                paddedImage[0][h][w] = 0.0;
                paddedImage[1][h][w] = 0.0;
                paddedImage[2][h][w] = 0.0;
            }
            else {
                paddedImage[0][h][w] = this->m_image[0][h][w];
                paddedImage[1][h][w] = this->m_image[1][h][w];
                paddedImage[2][h][w] = this->m_image[2][h][w];
            }
        }
    }

    return paddedImage;
}
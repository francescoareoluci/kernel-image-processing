#include <png++/png.hpp>
#include "image.h"

void threadConv(const MatrixChannels& sourceImage, 
                int line, 
                MatrixChannels& outImage, 
                const Matrix& mask,
                int width, int height, int channels, int filterWidth, int filterHeight,
                int threadsNumber);

Image::Image()
{
    m_threads = std::vector<std::thread>();
}

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

MatrixChannels Image::getImage() const
{
    return this->m_image;
}

bool Image::loadImage(const char *filename)
{
    // Load image
    png::image<png::gray_pixel> image(filename);

    // Build matrix from image
    MatrixChannels imageMatrix(1, Matrix(image.get_height(), Array(image.get_width())));
    
    for (int h = 0; h < image.get_height(); h++) {
        for (int w = 0; w < image.get_width(); w++) {
            imageMatrix[0][h][w] = image[h][w];
            //imageMatrix[1][h][w] = image[h][w].green;
            //imageMatrix[2][h][w] = image[h][w].blue;
        }
    }

    m_image = imageMatrix;

    return true;
}

bool Image::saveImage(const char *filename) const
{
    if (m_image.size() != 1)
    {
        std::cerr << "Unable to save image" << std::endl;
        return false;
    }

    int height = this->getImageHeight();
    int width = this->getImageWidth();

    png::image<png::gray_pixel> imageFile(width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            imageFile[y][x] = m_image[0][y][x];
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

    MatrixChannels newImage = applyFilterCommon(kernel);

    resultingImage.setImage(newImage);
    std::cout << "Done!" << std::endl;

    return true;
}

bool Image::applyFilter(const Kernel& kernel)
{
    std::cout << "Applying filter to image" << std::endl;
    
    MatrixChannels newImage = applyFilterCommon(kernel);
    if (newImage.empty()) {
        return false;
    }

    this->setImage(newImage);

    std::cout << "Done!" << std::endl;

    return true;
}

MatrixChannels Image::applyFilterCommon(const Kernel& kernel) const
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
        return MatrixChannels();
    }

    if (filterHeight == 0 || filterWidth == 0) {
        std::cerr << "Invalid filter dimension" << std::endl;
        return MatrixChannels();
    }

    // Input padding w.r.t. filter size
    MatrixChannels paddedImage = buildPaddedImage(filterHeight, filterWidth);

    MatrixChannels newImage(channels, Matrix(height, Array(width)));
    Matrix mask = kernel.getKernel();

    // Apply convolution
    for (int d = 0; d < channels; d++) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                for (int h = i;  h < i + filterHeight; h++) {
                    for (int w = j; w < j + filterWidth; w++) {
                        newImage[d][i][j] += mask[h - i][w - j] * paddedImage[d][h + int(filterHeight/2) + 1][w + int(filterWidth/2) + 1];
                    }
                }
            }
        }
    }

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
    MatrixChannels paddedImage = buildPaddedImage(filterHeight, filterWidth);
    MatrixChannels newImage(channels, Matrix(height, Array(width)));
    Matrix mask = kernel.getKernel();

    if (!m_threads.empty())
    {
        for (int i = 0; i < threadsNumber; i++) {
            m_threads[i] = (std::thread(threadConv, std::ref(paddedImage), 
                                    i, std::ref(newImage), std::ref(mask), 
                                    width, height, channels, filterWidth, filterHeight, threadsNumber));
        }
    }
    else {
        for (int i = 0; i < threadsNumber; i++) {
            m_threads.push_back(std::thread(threadConv, std::ref(paddedImage), 
                                    i, std::ref(newImage), std::ref(mask), 
                                    width, height, channels, filterWidth, filterHeight, threadsNumber));
        }
    }

    for (int i = 0; i < threadsNumber; i++) {
        m_threads[i].join();
    }

    resultingImage.setImage(newImage);

    std::cout << "Done!" << std::endl;
    //m_threads.clear();
    
    return true;
}

void threadConv(const MatrixChannels& sourceImage, 
                int line, 
                MatrixChannels& outImage, 
                const Matrix& mask,
                int width, int height, int channels, int filterWidth, int filterHeight,
                int threadsNumber)
{
    if (line < 0 || line > height)
    {
        return;
    }

    // Apply convolution
    for (int d = 0; d < channels; d++) {
        for (int l = line; l < height; l += threadsNumber) {
            if (l > height) {
                break;
            }
            for (int j = 0; j < width; j++) {
                for (int h = l;  h < l + filterHeight; h++) {
                    for (int w = j; w < j + filterWidth; w++) {
                        outImage[d][l][j] += mask[h - l][w - j] * sourceImage[d][h + int(filterHeight/2) + 1][w + int(filterWidth/2) + 1];
                    }
                }
            }
        }
    }
}

MatrixChannels Image::buildPaddedImage(const int paddingHeight,
                                        const int paddingWidth) const
{
    int height = this->getImageHeight();
    int width = this->getImageWidth();
    MatrixChannels paddedImage(1, Matrix(height + (paddingHeight * 2) - 1, 
                                        Array(width + (paddingWidth * 2) - 1)));

    for (int h = 0; h < paddedImage[0].size(); h++) {
        for (int w = 0; w < paddedImage[0][0].size(); w++) {
            if ((h < paddingHeight - 1) || (w < paddingWidth - 1) || 
                (h > height + paddingHeight - 1) || 
                (w > width + paddingWidth - 1)) {
                paddedImage[0][h][w] = 0.0;
                //paddedImage[1][h][w] = 0.0;
                //paddedImage[2][h][w] = 0.0;
            }
            else {
                paddedImage[0][h][w] = this->m_image[0][h - paddingHeight][w - paddingWidth];
                //paddedImage[1][h][w] = this->m_image[1][h][w];
                //paddedImage[2][h][w] = this->m_image[2][h][w];
            }
        }
    }

    /*
    int pheight = paddedImage[0].size();
    int pwidth = paddedImage[0][0].size();

    png::image<png::gray_pixel> imageFile(pwidth, pheight);

    for (int y = 0; y < pheight; y++) {
        for (int x = 0; x < pwidth; x++) {
            imageFile[y][x] = paddedImage[0][y][x];
            //imageFile[y][x].green = m_image[1][y][x];
            //imageFile[y][x].blue = m_image[2][y][x];
        }
    }
    imageFile.write("padded.png");

    std::cout << "Image saved in " << std::string("padded.png") << std::endl;
    */

    return paddedImage;
}
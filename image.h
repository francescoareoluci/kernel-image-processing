#include <vector>
#include <thread>
#include "types.h"
#include "kernel.h"

class Image
{
    public:
        Image();

        /*
         *  @brief: Dtor
         */ 
        ~Image() {
            m_image.clear();
            MatrixChannels().swap(m_image);
        }

        /*
         * @brief: get loaded image width
         */ 
        int getImageWidth() const;

        /*
         * @brief: get loaded image height
         */
        int getImageHeight() const;

        /*
         * @brief: get number of channels of the image
         */
        int getImageChannels() const;

        /*
         * @brief: set the image given another MatrixChannel
         * 
         * @params: source: the matrix to be set as state
         * @return: true is successfull, false otherwise
         */
        bool setImage(const MatrixChannels& source);

        /*
         * @brief: return the matrix state
         * 
         * @return: the matrix state
         */
        MatrixChannels getImage() const;

        /*
         * @brief: load an image from filename path
         * 
         * @params: filename: the path of the image to be loaded
         * @return: true is successfull, false otherwise
         */
        bool loadImage(const char *filename);   

        /*
         * @brief: save an image in filename path
         *
         * @params: filename: the path where to save the image
         * @return: true is successfull, false otherwise
         */
        bool saveImage(const char *filename) const;

        /*
         * @brief: apply a kernel to the image and pass 
         *         result in resultingImage object
         * 
         * @params[out]: resultingImage: the image object where the matrix will be saved
         * @params[in]: kernel: kernel to be applied to the image
         * @return: true if successful, false otherwise
         */
        bool applyFilter(Image& resultingImage, const Kernel& kernel) const;

        /*
         * @brief: apply a kernel to the image and save it's state.
         *          This method can be used to iterate a convolution
         *          on the same image more than 1 times.
         * 
         * @params[in]: kernel: kernel to be applied to the image
         * @return: true if successful, false otherwise
         */
        bool applyFilter(const Kernel& kernel);

        bool multithreadFiltering(Image& resultingImage, const Kernel& kernel, int threadsNumber);

    private:
        /*
         * @brief: A common method to apply the kernel to the image
         */
        MatrixChannels applyFilterCommon(const Kernel& kernel) const;

        /*
         * @brief: return a padded matrix using matrix state 
         *          and requested padding
         */
        MatrixChannels buildPaddedImage(const int paddingHeight,
                                        const int paddingWidth) const;

        MatrixChannels m_image;
        std::vector<std::thread> m_threads;
};
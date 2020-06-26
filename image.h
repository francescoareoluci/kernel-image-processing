#include <vector>
#include <assert.h>
#include "types.h"
#include "kernel.h"

class Image
{
    public:
        Image();

        /*
         * @brief: get loaded image width
         */ 
        int getImageWidth() const;

        /*
         * @brief: get loaded image height
         */
        int getImageHeight() const;


        int getImageChannels() const;

        bool setImage(const MatrixChannels& source);

        /*
         * @brief: load an image from filename path
         */
        bool loadImage(const char *filename);   

        /*
         * @brief: save an image in filename path
         */
        bool saveImage(const char *filename) const;

        /*
         * @brief apply a kernel to the image and pass result in resultingImage
         */
        bool applyFilter(Image& resultingImage, const Kernel& kernel) const;

    private:
        MatrixChannels buildPaddedImage(const int startingHeight, 
                                        const int startingWidth,
                                        const int paddingHeight,
                                        const int paddingWidth) const;

        MatrixChannels m_image;

};
#include <vector>
#include <assert.h>
#include "types.h"

class Kernel 
{
    public:
        Kernel();

        /*
         *  @brief: Dtor
         */ 
        ~Kernel() {
            m_filterMatrix.clear();
            Matrix().swap(m_filterMatrix);
        }

        /*
         * @brief: Print the kernel in the command line
         */
        void printKernel() const;

        /*
         * @brief: Set up the Kernel object as a Gaussian filter
         * 
         * @param: height: integer height of the filter
         * @param: width: integer width of the filter
         * @param: stdDev: standard deviation
         * @return: true for successful setup, false otherwise
         */
        bool setGaussianFilter(const int height, const int width, const double stdDev);

        /*
         * @brief: Set up the Kernel object as a sharpener filter
         * 
         * @param: height: integer height of the filter
         * @param: width: integer width of the filter
         * @param: max: max value of the filter
         * @param: min: min value of the filter
         * @return: true for successful setup, false otherwise
         */
        bool setSharpenFilter(int height, int width, int max, int min);

        /*
         * @brief: Set up the Kernel object as an edge detector filter
         * 
         * @param: height: integer height of the filter
         * @param: width: integer width of the filter
         * @param: max: max value of the filter
         * @param: min: min value of the filter
         * @return: true for successful setup, false otherwise
         */
        bool setEdgeDetectionFilter(int height, int width, int max, int min);

        /*
         * @brief: Set up the Kernel object as an alternative edge detector filter
         * 
         * @param: height: integer height of the filter
         * @param: width: integer width of the filter
         * @param: max: max value of the filter
         * @param: min: min value of the filter
         * @return: true for successful setup, false otherwise
         */
        bool setAltEdgeDetectionFilter(int height, int width, int max, int min);

        /*
         * @brief: return the kernel width
         */
        int getKernelWidth() const;

        /*
         * @brief: return the kernel height
         */
        int getKernelHeight() const;

        /*
         * @brief: return the kernel as a matrix
         */
        Matrix getKernel() const;

    private:
        /*
         * @brief: A common method to build the kernel
         */
        bool buildKernelCommon(Matrix &kernel, int max, int min);

        Matrix m_filterMatrix;
};
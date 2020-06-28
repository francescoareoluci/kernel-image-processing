#include <vector>


class Kernel 
{
    public:
        Kernel();

        /*
         *  @brief: Dtor
         */ 
        ~Kernel() {
            m_filterMatrix.clear();
            std::vector<double>().swap(m_filterMatrix);
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
        std::vector<double> getKernel() const;

    private:
        /*
         * @brief: A common method to build the kernel
         */
        bool buildKernelCommon(std::vector<double> &kernel, int max, int min, int height, int width);

        std::vector<double> m_filterMatrix;
        int m_filterWidth;
        int m_filterHeight;
};
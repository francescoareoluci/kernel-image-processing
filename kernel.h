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
         * @return: true for successful setup, false otherwise
         */
        bool setSharpenFilter();

        /*
         * @brief: Set up the Kernel object as an edge detector filter
         * 
         * @return: true for successful setup, false otherwise
         */
        bool setEdgeDetectionFilter();

        /*
         * @brief: Set up the Kernel object as a Laplacian filter
         * 
         * @return: true for successful setup, false otherwise
         */
        bool setLaplacianFilter();

        /*
         * @brief: Set up the Kernel object as a Gaussian of Laplacian filter
         * 
         * @return: true for successful setup, false otherwise
         */
        bool setGaussianLaplacianFilter();

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

        std::vector<double> m_filterMatrix;     ///< Linearized matrix containing the kernel 
        int m_filterWidth;                      ///< Kernel height
        int m_filterHeight;                     ///< Kernel width
};
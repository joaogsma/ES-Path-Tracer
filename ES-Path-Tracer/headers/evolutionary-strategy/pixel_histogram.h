#ifndef __GUARD_PIXEL_HISTOGRAM_H__
#define __GUARD_PIXEL_HISTOGRAM_H__

#include <map>

namespace ES
{
    namespace fitness
    {
        class Pixel_Histogram {
        public:
            // Increases counter for the specified color
            void increase(unsigned int red, unsigned int green, unsigned int blue);

            /*  Computation of the mean color based on the probabilities in the 
                histogram. This is equivalent to the summation of (p * color),
                for all values of color and its corresponding estimated 
                probabilities p */
            void mean_color(unsigned int& red, unsigned int& green, 
                unsigned int& blue) const;

            /*  Computation of the information quantity associated with a given color. 
                The information quantity is defined as -log(p), where p is the 
                color's estimated probability */
            double information_quantity(unsigned int red, unsigned int green, 
                unsigned int blue);

        private:
            std::map<unsigned int, std::map<unsigned int, 
                std::map<unsigned int, unsigned int>>> color_histogram;
            long total_samples;
        };
    }
}

#endif
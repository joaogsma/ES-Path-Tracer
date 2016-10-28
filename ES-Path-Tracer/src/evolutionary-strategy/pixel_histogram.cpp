#include <cmath>
#include <map>

#include "evolutionary-strategy/pixel_histogram.h"

using std::log;
using std::map;
using std::round;

namespace ES
{
    namespace fitness
    {
        void Pixel_Histogram::mean_color(unsigned int& red, 
            unsigned int& green, unsigned int& blue) const
        {
            typedef map<unsigned int, map<unsigned int, map<unsigned int, unsigned int>>> red_map;
            typedef map<unsigned int, map<unsigned int, unsigned int>> green_map;
            typedef map<unsigned int, unsigned int> blue_map;

            double mean_red = 0, mean_green = 0, mean_blue = 0;

            // Iterate through all colors in the histogram
            for (red_map::const_iterator red_it = color_histogram.begin();
                red_it != color_histogram.end(); red_it++)
            {
                unsigned int red_color = red_it->first;

                for (green_map::const_iterator green_it = red_it->second.begin();
                    green_it != red_it->second.end(); green_it++)
                {
                    unsigned int green_color = green_it->first;

                    for (blue_map::const_iterator blue_it = green_it->second.begin();
                        blue_it != green_it->second.end(); blue_it++)
                    {
                        unsigned int blue_color = blue_it->first;
                        long double count = blue_it->second;

                        // Estimated percentage of this color
                        double probability = count / total_samples;

                        // Add the color's contribution to the mean
                        mean_red += red_color * probability;
                        mean_green += green_color * probability;
                        mean_blue += blue_color * probability;
                    }
                }
            }

            red = (unsigned int) round(mean_red);
            green = (unsigned int) round(mean_green);
            blue = (unsigned int) round(mean_blue);
        }

        double Pixel_Histogram::information_quantity(unsigned int red, 
            unsigned int green, unsigned int blue)
        {
            long double count = color_histogram[red][green][blue];
            double probability = count / total_samples;

            return (-1) * log(probability);
        }
    }
}
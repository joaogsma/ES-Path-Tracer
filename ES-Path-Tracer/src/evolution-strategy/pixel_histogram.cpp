#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <stdexcept>

#include "evolution-strategy/pixel_histogram.h"
#include "shading/color3.h"

namespace es
{
    namespace fitness
    {
        void Pixel_Histogram::increase(int red, int green, int blue)
        {
			if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255)
				throw std::invalid_argument("invalid color");

			++m_color_histogram[to_hash_key(red, green, blue)];
            ++m_total_samples;
        }

        Color3 Pixel_Histogram::weighted_mean_color() const
        {
			Color3 weighted_sum(0.0);

			for (const std::unordered_map<int, int>::value_type& entry : m_color_histogram)
			{
				const Color3& color = from_hash_key(entry.first);
				weighted_sum += entry.second * color;
			}

			return weighted_sum / m_total_samples;
        }

        double Pixel_Histogram::information_quantity(int red, int green, int blue, int distance)
        {
            if (distance <= 0)
                throw std::invalid_argument("Distance must be positive");

            int distance2 = distance * distance;
            int total_count = 0;

            int min_red = std::max(red - distance, 0);
            int max_red = std::min(red + distance, 256) - 1;
            int min_green = std::max(green - distance, 0);
            int max_green = std::min(green + distance, 256) - 1;
            int min_blue = std::max(blue - distance, 0);
            int max_blue = std::min(blue + distance, 256) - 1;

            for (int r = min_red; r <= max_red; ++r)
            {
                for (int g = min_green; g <= max_green; ++g)
                {
                    for (int b = min_blue; b <= max_blue; ++b)
                    {
                        int dist = pow(r - red, 2) + pow(g - green, 2) + pow(b - blue, 2);
                        if (dist <= distance2)
                            total_count += m_color_histogram[to_hash_key(r, g, b)];
                    }
                }
            }
			
            double probability = double(total_count) / m_total_samples;
            return (-1) * log(probability);
        }

		int Pixel_Histogram::to_hash_key(int red, int green, int blue) const
		{
			return (red * 256 * 256) + (green * 256) + blue;
		}

		Color3 Pixel_Histogram::from_hash_key(int key) const
		{
			int red = key / (256 * 256);
			key %= 256 * 256;
			int green = key / 256;
			key %= 256;
			int blue = key;
			return Color3(red, green, blue);
		}
    }
}
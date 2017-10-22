#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <stdexcept>

#include "path-tracer/color_histogram.h"
#include "shading/color3.h"

void Color_Histogram::increase(int red, int green, int blue)
{
	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255)
		throw std::invalid_argument("invalid color");

	++m_color_histogram[to_hash_key(red, green, blue)];
    ++m_total_samples;
}

Color3 Color_Histogram::weighted_mean_color() const
{
	Color3 weighted_sum(0.0);

	for (const std::unordered_map<int, int>::value_type& entry : m_color_histogram)
	{
		const Color3& color = from_hash_key(entry.first);
		weighted_sum += entry.second * color;
	}

	return weighted_sum / m_total_samples;
}

double Color_Histogram::information_quantity(int red, int green, int blue, int radius)
    {
        if (radius <= 0)
            throw std::invalid_argument("Distance must be positive");

        int radius2 = radius * radius;
        int total_count = 0;

        int min_red = std::max(red - radius, 0);
        int max_red = std::min(red + radius, 256) - 1;
        int min_green = std::max(green - radius, 0);
        int max_green = std::min(green + radius, 256) - 1;
        int min_blue = std::max(blue - radius, 0);
        int max_blue = std::min(blue + radius, 256) - 1;

        for (int r = min_red; r <= max_red; ++r)
        {
            for (int g = min_green; g <= max_green; ++g)
            {
                for (int b = min_blue; b <= max_blue; ++b)
                {
                    int dist = int(pow(r - red, 2) + pow(g - green, 2) + pow(b - blue, 2));
                    if (dist <= radius2)
                        total_count += m_color_histogram[to_hash_key(r, g, b)];
                }
            }
        }
		
        double probability = double(total_count) / m_total_samples;
        return (-1) * log(probability);
    }

int Color_Histogram::to_hash_key(int red, int green, int blue) const
{
	return (red * 256 * 256) + (green * 256) + blue;
}

Color3 Color_Histogram::from_hash_key(int key) const
{
	int red = key / (256 * 256);
	key %= 256 * 256;
	int green = key / 256;
	key %= 256;
	int blue = key;
	return Color3(red, green, blue);
}

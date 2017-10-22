#include <stdexcept>

#include "evolution-strategy/fitness.h"
#include "evolution-strategy/individual.h"

namespace es
{
	void Color_Histogram_Fitness::set_color_histogram(const Color_Histogram& histogram)
	{
		m_color_histogram = histogram;
	}
	
	void Color_Histogram_Fitness::set_radius(int radius)
	{
		if (radius <= 0)
			throw std::invalid_argument("Distance must be positive");
		m_radius = radius;
	}

	double Color_Histogram_Fitness::operator()(const Individual& individual)
	{
		// TODO: path trace...
	}
}
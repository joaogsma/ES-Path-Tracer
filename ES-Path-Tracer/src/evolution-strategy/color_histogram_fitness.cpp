#include <stdexcept>

#include "evolution-strategy/color_histogram_fitness.h"
#include "evolution-strategy/individual.h"
#include "path-tracer/color_histogram.h"
#include "path-tracer/path_tracer.h"

namespace es
{
	Color_Histogram_Fitness::Color_Histogram_Fitness(const Path_Tracer& path_tracer, int radius)
		: m_path_tracer(path_tracer)
	{
		set_radius(radius);
	}

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
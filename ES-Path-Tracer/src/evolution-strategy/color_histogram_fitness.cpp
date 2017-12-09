#include <algorithm>
#include <stdexcept>

#include "evolution-strategy/color_histogram_fitness.h"
#include "evolution-strategy/individual.h"
#include "geometry/ray.h"
#include "path-tracer/color_histogram.h"
#include "path-tracer/path_tracer.h"
#include "random/es_individual_random_sequence.h"
#include "random/random_sequence.h"
#include "shading/color3.h"

namespace es
{
	Color_Histogram_Fitness::Color_Histogram_Fitness(
		const Path_Tracer& path_tracer,
		Color_Histogram* color_histogram,
		const Ray& ray,
		int radius)
		: m_path_tracer(&path_tracer),
		m_color_histogram(color_histogram),
		m_ray(ray)
	{
		set_radius(radius);
	}

	void Color_Histogram_Fitness::set_color_histogram(Color_Histogram* histogram)
	{
		m_color_histogram = histogram;
	}
	
	void Color_Histogram_Fitness::set_radius(int radius)
	{
		if (radius <= 0)
			throw std::invalid_argument("Distance must be positive");
		m_radius = radius;
	}

	double Color_Histogram_Fitness::operator()(Individual& individual)
	{
		random::Random_Sequence& random_sequence = 
			random::ES_Individual_Random_Sequence(individual);
		const Radiance3& path_tracer_radiance = 
			m_path_tracer->path_trace(m_ray, random_sequence, true);
		const Radiance3& gamma_corrected_radiance = 
			m_path_tracer->gamma_correction(path_tracer_radiance);

		m_color_histogram->increase(
			(int) gamma_corrected_radiance.r,
			(int) gamma_corrected_radiance.g,
			(int) gamma_corrected_radiance.b);

		const double maximum_radiance = 
			*std::max_element(gamma_corrected_radiance.begin(), gamma_corrected_radiance.end());

		const double information_quantity = m_color_histogram->information_quantity(
			(int)gamma_corrected_radiance.r,
			(int)gamma_corrected_radiance.g,
			(int)gamma_corrected_radiance.b,
			m_radius);
		return maximum_radiance * information_quantity;
	}
}
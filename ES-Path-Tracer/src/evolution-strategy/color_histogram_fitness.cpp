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

// "inf quantity"
// "none"
const std::string FREQUENCY_COEFFICIENT = "inf quantity";
// "maximum"
// "norm maximum"
// "mean"
// "norm mean"
// "norm mean and maximum"
// "none"
const std::string RADIANCE_COEFFICIENT = "norm mean and maximum";

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

		///////////////////////////////////////////////////
		//// Radiance coefficient
		///////////////////////////////////////////////////

		double radiance_coefficient;

		if (RADIANCE_COEFFICIENT == "none")
		{
			radiance_coefficient = 1.0;
		}
		else if (RADIANCE_COEFFICIENT == "maximum")
		{
			const double maximum_radiance = 
				*std::max_element(gamma_corrected_radiance.begin(), gamma_corrected_radiance.end());
			radiance_coefficient = maximum_radiance;
		}
		else if (RADIANCE_COEFFICIENT == "norm maximum")
		{
			const double maximum_radiance =
				*std::max_element(gamma_corrected_radiance.begin(), gamma_corrected_radiance.end());
			radiance_coefficient = maximum_radiance / 255.0;
		}
		else if (RADIANCE_COEFFICIENT == "mean")
		{
			const double mean_radiance =
				(gamma_corrected_radiance.r + gamma_corrected_radiance.g + gamma_corrected_radiance.b) / 3.0;
			radiance_coefficient = mean_radiance;
		}
		else if (RADIANCE_COEFFICIENT == "norm mean")
		{
			const double mean_radiance =
				(gamma_corrected_radiance.r + gamma_corrected_radiance.g + gamma_corrected_radiance.b) / 3.0;
			radiance_coefficient = mean_radiance / 255.0;
		}
		else if (RADIANCE_COEFFICIENT == "norm mean and maximum")
		{
			const double maximum_radiance =
				*std::max_element(gamma_corrected_radiance.begin(), gamma_corrected_radiance.end());
			const double mean_radiance =
				(gamma_corrected_radiance.r + gamma_corrected_radiance.g + gamma_corrected_radiance.b) / 3.0;
			radiance_coefficient = (mean_radiance / 255.0) * (maximum_radiance / 255.0);
		}
		else
		{
			throw std::runtime_error("Unknown configuration");
		}
		
		///////////////////////////////////////////////////
		//// Frequency coefficient
		///////////////////////////////////////////////////

		double frequency_coefficient;

		if (FREQUENCY_COEFFICIENT == "none")
		{
			frequency_coefficient = 1.0;
		}
		else if (FREQUENCY_COEFFICIENT == "inf quantity")
		{
			const double information_quantity = m_color_histogram->information_quantity(
				(int) gamma_corrected_radiance.r,
				(int) gamma_corrected_radiance.g,
				(int) gamma_corrected_radiance.b,
				m_radius);
			frequency_coefficient = information_quantity;
		}
		else
		{
			throw std::runtime_error("Unknown configuration");
		}

		return radiance_coefficient * frequency_coefficient;
	}
}
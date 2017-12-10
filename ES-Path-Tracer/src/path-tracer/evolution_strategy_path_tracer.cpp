#include <algorithm>  // test
#include <iostream> // test
#include <utility>  // test
#include <string>
#include <stdexcept>

#include "evolution-strategy/color_histogram_fitness.h"
#include "evolution-strategy/evolution_strategy.h"
#include "evolution-strategy/individual.h"
#include "evolution-strategy/parent_selection.h"
#include "evolution-strategy/recombination.h"
#include "evolution-strategy/stop_condition.h"
#include "evolution-strategy/survivor_selection.h"
#include "geometry/ray.h"
#include "path-tracer/camera.h"
#include "path-tracer/evolution_strategy_path_tracer.h"
#include "path-tracer/path_tracer.h"
#include "random/es_individual_random_sequence.h"  // test
#include "scene/scene.h"
#include "shading/color3.h"

#define HEURISTIC 3

const int RADIUS_SIZE = 4;
// "inv inf quantity"
// "none"
const std::string FREQUENCY_COEFFICIENT = "none";
// "maximum"
// "norm maximum"
// "mean"
// "norm mean"
// "norm mean and maximum"
// "none"
const std::string RADIANCE_COEFFICIENT = "norm mean and maximum";
const bool DECREASING_ORDER = true;

Evolution_Strategy_Path_Tracer::Evolution_Strategy_Path_Tracer(
	const Camera* camera,
	const scene::Scene* scene,
	double window_width,
	double aspect_ratio,
	int resolution_width,
	int max_iterations_per_pixel,
	double children_population_ratio,
	int population_size,
	double gamma_coefficient,
	double gamma_exponent,
	int num_threads)
	: Path_Tracer(
		camera,
		scene,
		window_width,
		aspect_ratio,
		resolution_width,
		gamma_coefficient,
		gamma_exponent,
		num_threads)
{
	set_max_iterations_per_pixel(max_iterations_per_pixel);
	set_children_population_ratio(children_population_ratio);
	set_population_size(population_size);
}

void Evolution_Strategy_Path_Tracer::set_max_iterations_per_pixel(int max_iterations_per_pixel)
{
	if (max_iterations_per_pixel <= 0)
		throw std::invalid_argument("Invalid maximum iterations number");
	m_max_iterations_per_pixel = max_iterations_per_pixel;
}

void Evolution_Strategy_Path_Tracer::set_children_population_ratio(
	double children_population_ratio)
{
	if (children_population_ratio <= 0)
		throw std::invalid_argument("Invalid children to population size ratio");
	m_children_population_ratio = children_population_ratio;
}

void Evolution_Strategy_Path_Tracer::set_population_size(int population_size)
{
	if (population_size < 2)
		throw std::invalid_argument("Invalid population size");
	m_population_size = population_size;
}

Radiance3 Evolution_Strategy_Path_Tracer::estimate_pixel_color(const Ray& ray) const
{
	Color_Histogram color_histogram;
	es::Evolution_Strategy::fitness_function color_histogram_fitness_function =
		es::Color_Histogram_Fitness(*this, &color_histogram, ray, RADIUS_SIZE);
	es::Evolution_Strategy::parent_selection_function global_uniform_parent_selection_function =
		es::Parent_Selection::global_uniform_selection;
	es::Evolution_Strategy::recombination_function hibrid_recombination_function =
		es::Recombination::hibrid_recombination;
	es::Evolution_Strategy::survivor_selection_function generational_selection_function =
		es::Survivor_Selection::generational_selection;
	es::Evolution_Strategy::stop_condition_function max_iterations_stop_condition =
		es::Stop_Condition::max_iterations_stop;

	const int initial_individual_length = 3;

	es::Evolution_Strategy evolution_strategy(
		m_max_iterations_per_pixel,
		initial_individual_length,
		m_children_population_ratio,
		m_population_size,
		max_iterations_stop_condition,
		global_uniform_parent_selection_function,
		hibrid_recombination_function,
		&color_histogram_fitness_function,
		generational_selection_function);

	evolution_strategy.evolve();

	// Heuristic #1:
	// Return the weighted mean of all radiances gathered in the histogram
	if (HEURISTIC == 1)
	{
		return color_histogram.weighted_mean_color();
	}

	// Heuristic #2:
	// Return the mean radiance of individuals in the final population
	if (HEURISTIC == 2)
	{
		Radiance3 accumulator(0.0);
		for (es::Individual individual : evolution_strategy.population())
		{
			random::ES_Individual_Random_Sequence individual_random_sequence(individual);
			const Radiance3& path_tracer_radiance = path_trace(ray, individual_random_sequence, true);
			accumulator += path_tracer_radiance;
		}
		return gamma_correction(accumulator / evolution_strategy.population_size());
	}

	// Heuristic #3:
	// Return the weighted mean of the most frequent colors
	if (HEURISTIC == 3)
	{
		std::vector<int> colors;

		for (const std::pair<int, unsigned int>& element : color_histogram.m_color_histogram)
		{
			if (element.second > 0)
				colors.push_back(element.first);
		}

		std::sort(
			colors.begin(),
			colors.end(),
			[&](int color0, int color1) { return color_compare_predicate(color0, color1, color_histogram); });

		const int reduced_size = std::max(1, int(0.99 * colors.size()));
		colors.erase(colors.begin() + reduced_size, colors.end());

		Radiance3 accumulator(0.0);
		int n_samples = 0;
		for (int color : colors)
		{
			int occurrences = color_histogram.m_color_histogram[color];
			accumulator += color_histogram.m_color_histogram[color] * color_histogram.from_hash_key(color);
			n_samples += occurrences;
		}

		return accumulator / n_samples;
	}
}

bool Evolution_Strategy_Path_Tracer::color_compare_predicate(
	int color0,
	int color1,
	const Color_Histogram& color_histogram)
{
	const Radiance3 radiance0 = color_histogram.from_hash_key(color0);
	const Radiance3 radiance1 = color_histogram.from_hash_key(color1);

	double frequency_coefficient0;
	double frequency_coefficient1;

	if (FREQUENCY_COEFFICIENT == "none")
	{
		frequency_coefficient0 = 1.0;
		frequency_coefficient1 = 1.0;
	}
	else if (FREQUENCY_COEFFICIENT == "inv inf quantity")
	{
		const double information_quantity0 = color_histogram.information_quantity(
			(int) radiance0.r,
			(int) radiance0.g,
			(int) radiance0.b);
		const double information_quantity1 = color_histogram.information_quantity(
			(int) radiance1.r,
			(int) radiance1.g,
			(int) radiance1.b);
		frequency_coefficient0 = 1.0 / information_quantity0;
		frequency_coefficient1 = 1.0 / information_quantity1;
	}
	else
	{
		throw std::runtime_error("Unknown configuration");
	}
	
	double radiance_coefficient0;
	double radiance_coefficient1;

	if (RADIANCE_COEFFICIENT == "none")
	{
		radiance_coefficient0 = 1.0;
		radiance_coefficient1 = 1.0;
	}
	else if (RADIANCE_COEFFICIENT == "mean")
	{
		const double mean_radiance0 = (radiance0.r + radiance0.g + radiance0.b) / 3.0;
		const double mean_radiance1 = (radiance1.r + radiance1.g + radiance1.b) / 3.0;
		radiance_coefficient0 = mean_radiance0;
		radiance_coefficient1 = mean_radiance1;
	}
	else if (RADIANCE_COEFFICIENT == "norm mean")
	{
		const double mean_radiance0 = (radiance0.r + radiance0.g + radiance0.b) / 3.0;
		const double mean_radiance1 = (radiance1.r + radiance1.g + radiance1.b) / 3.0;
		radiance_coefficient0 = mean_radiance0 / 255.0;
		radiance_coefficient1 = mean_radiance1 / 255.0;
	}
	else if (RADIANCE_COEFFICIENT == "maximum")
	{
		const double max_radiance0 = std::max(radiance0.r, std::max(radiance0.g, radiance0.b));
		const double max_radiance1 = std::max(radiance1.r, std::max(radiance1.g, radiance1.b));
		radiance_coefficient0 = max_radiance0;
		radiance_coefficient1 = max_radiance1;
	}
	else if (RADIANCE_COEFFICIENT == "norm maximum")
	{
		const double max_radiance0 = std::max(radiance0.r, std::max(radiance0.g, radiance0.b));
		const double max_radiance1 = std::max(radiance1.r, std::max(radiance1.g, radiance1.b));
		radiance_coefficient0 = max_radiance0 / 255.0;
		radiance_coefficient1 = max_radiance1 / 255.0;
	}
	else if (RADIANCE_COEFFICIENT == "norm mean and maximum")
	{
		const double mean_radiance0 = (radiance0.r + radiance0.g + radiance0.b) / 3.0;
		const double mean_radiance1 = (radiance1.r + radiance1.g + radiance1.b) / 3.0;
		const double max_radiance0 = std::max(radiance0.r, std::max(radiance0.g, radiance0.b));
		const double max_radiance1 = std::max(radiance1.r, std::max(radiance1.g, radiance1.b));
		radiance_coefficient0 = (mean_radiance0 / 255.0) * (max_radiance0 / 255.0);
		radiance_coefficient1 = (mean_radiance1 / 255.0) * (max_radiance1 / 255.0);
	}
	else
	{
		throw std::runtime_error("Unknown configuration");
	}
	
	bool increasing_order_result = 
		radiance_coefficient0 * frequency_coefficient0 < radiance_coefficient1 * frequency_coefficient1;

	return DECREASING_ORDER ? !increasing_order_result : increasing_order_result;
}
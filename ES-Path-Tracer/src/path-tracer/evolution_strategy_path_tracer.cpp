#include <stdexcept>

#include "evolution-strategy/color_histogram_fitness.h"
#include "evolution-strategy/evolution_strategy.h"
#include "evolution-strategy/parent_selection.h"
#include "evolution-strategy/recombination.h"
#include "evolution-strategy/stop_condition.h"
#include "evolution-strategy/survivor_selection.h"
#include "geometry/ray.h"
#include "path-tracer/camera.h"
#include "path-tracer/evolution_strategy_path_tracer.h"
#include "path-tracer/path_tracer.h"
#include "scene/scene.h"
#include "shading/color3.h"

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
		es::Color_Histogram_Fitness(*this, &color_histogram, ray);
	es::Evolution_Strategy::parent_selection_function global_uniform_parent_selection_function =
		es::Parent_Selection::global_uniform_selection;
	es::Evolution_Strategy::recombination_function hibrid_recombination_function =
		es::Recombination::hibrid_recombination;
	es::Evolution_Strategy::survivor_selection_function generational_selection_function =
		es::Survivor_Selection::generational_selection;
	es::Evolution_Strategy::stop_condition_function max_iterations_stop_condition =
		es::Stop_Condition::max_iterations_stop;

	es::Evolution_Strategy evolution_strategy(
		100,
		15,
		7,
		10,
		max_iterations_stop_condition,
		global_uniform_parent_selection_function,
		hibrid_recombination_function,
		&color_histogram_fitness_function,
		generational_selection_function);

	evolution_strategy.evolve();
	return color_histogram.weighted_mean_color();
}
#include <stdexcept>

#include "geometry/ray.h"
#include "path-tracer/camera.h"
#include "path-tracer/path_tracer.h"
#include "path-tracer/monte_carlo_path_tracer.h"
#include "random/uniform_random_sequence.h"
#include "scene/scene.h"
#include "shading/color3.h"


Monte_Carlo_Path_Tracer::Monte_Carlo_Path_Tracer(
	const Camera* camera,
	const scene::Scene* scene,
	double window_width,
	double aspect_ratio,
	int resolution_width,
	int samples_per_pixel,
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
		set_samples_per_pixel(samples_per_pixel);
	}

void Monte_Carlo_Path_Tracer::set_samples_per_pixel(int samples_per_pixel)
{
	if (samples_per_pixel <= 0)
		throw std::invalid_argument("Number of samples per pixel must be positive");
	m_samples_per_pixel = samples_per_pixel;
}

Radiance3 Monte_Carlo_Path_Tracer::estimate_pixel_color(const Ray& ray) const
{
	random::Uniform_Random_Sequence random_seq;
	Radiance3 sample_estimate_sum = 0;
	for (int i = 0; i < m_samples_per_pixel; ++i)
		sample_estimate_sum += path_trace(ray, random_seq, true);
	return gamma_correction(sample_estimate_sum / m_samples_per_pixel);
}

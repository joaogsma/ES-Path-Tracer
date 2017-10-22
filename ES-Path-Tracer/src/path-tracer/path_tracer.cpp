#define _USE_MATH_DEFINES

#define PRINT_PROGRESS true

#include <algorithm>
#include <cmath>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "geometry/ray.h"
#include "geometry/vector3.h"
#include "path-tracer/camera.h"
#include "path-tracer/path_tracer.h"
#include "random/random_number_engine.h"
#include "random/random_sequence.h"
#include "random/uniform_random_sequence.h"
#include "scene/area_light.h"
#include "scene/scene.h"
#include "shading/color3.h"
#include "shading/surface_element.h"

int depth = 0;

Path_Tracer::Path_Tracer(
	const Camera* camera,
	const scene::Scene* scene,
	double window_width,
	double aspect_ratio,
	int resolution_width,
	int samples_per_pixel,
	double gamma_coefficient,
	double gamma_exponent,
	int num_threads)
{
	set_camera(camera);
	set_scene(scene);
	set_window_width(window_width);
	set_aspect_ratio(aspect_ratio);
	set_resolution_width(resolution_width);
	set_samples_per_pixel(samples_per_pixel);
	set_gamma_coefficient(gamma_coefficient);
	set_gamma_exponent(gamma_exponent);
	set_num_threads(num_threads);
}

Path_Tracer::Path_Tracer(const Path_Tracer& other) : m_pixel_lock(), m_image_lock()
{
	set_camera(other.m_camera);
	set_scene(other.m_scene);
	set_window_width(other.m_window_width);
	set_aspect_ratio(other.m_aspect_ratio);
	set_resolution_width(other.m_resolution_width);
	set_samples_per_pixel(other.m_samples_per_pixel);
	set_num_threads(other.m_num_threads);
}

void Path_Tracer::set_camera(const Camera* camera)
{
	if (!camera)
		throw std::invalid_argument("Null pointer");
	m_camera = camera;
}

void Path_Tracer::set_scene(const scene::Scene* scene)
{
	if (!scene)
		throw std::invalid_argument("Null pointer");
	m_scene = scene;
}

void Path_Tracer::set_window_width(double window_width)
{
	if (window_width <= 0)
		throw std::invalid_argument("Window width must be positive");
	m_window_width = window_width;
}

void Path_Tracer::set_resolution_width(int resolution_width)
{
	if (resolution_width <= 0)
		throw std::invalid_argument("Resolution width must be positive");
	m_resolution_width = resolution_width;
}

void Path_Tracer::set_aspect_ratio(double aspect_ratio)
{
	if (aspect_ratio <= 0)
		throw std::invalid_argument("Aspect ratio must be positive");
	m_aspect_ratio = aspect_ratio;
}

void Path_Tracer::set_samples_per_pixel(int samples_per_pixel)
{
	if (samples_per_pixel <= 0)
		throw std::invalid_argument("Number of samples per pixel must be positive");
	m_samples_per_pixel = samples_per_pixel;
}

void Path_Tracer::set_gamma_coefficient(double coefficient)
{
	if (coefficient <= 0)
		throw std::invalid_argument("Coefficient must be positive");
	m_gamma_coefficient = coefficient;
}

void Path_Tracer::set_gamma_exponent(double gamma_exponent)
{
	if (gamma_exponent <= 0)
		throw std::invalid_argument("Exponent must be greater than 0");
	m_gamma_exponent = gamma_exponent;
}

void Path_Tracer::set_num_threads(int num_threads)
{
	if (num_threads <= 0)
		throw std::invalid_argument("Number of threads must be positive");
	m_num_threads = num_threads;
}

void Path_Tracer::compute_image(std::vector<std::vector<Radiance3>>& image)
{
	const int resolution_height = (int) round(m_resolution_width / m_aspect_ratio);
	const std::vector<Radiance3> empty_row(m_resolution_width, Radiance3(0.0));
	image = std::vector<std::vector<Radiance3>>(resolution_height, empty_row);

	std::vector<std::thread> threads;
	while (threads.size() < m_num_threads)
		threads.push_back(std::thread(&Path_Tracer::thread_code, this, &image));

	for (std::thread& thread : threads)
		thread.join();

	m_current_row = 0;

	if (PRINT_PROGRESS)
		std::cout << std::endl;
}

Radiance3 Path_Tracer::path_trace(
	const Ray& ray,
	random::Random_Sequence& random_seq,
	bool is_eye_ray,
	double refractive_index)
{
	/*  Compute the radiance BACK along the given ray. In the event that the ray is an eye-ray, 
        include light emitted by the first surface encountered. For subsequent rays, such light 
        has already been counted in the computation of direct lighting at prior hits */
    
	Radiance3 l_o(0.0);

    scene::Surface_Element surfel;

    double dist = std::numeric_limits<double>::infinity();

	if (!m_scene->intersect(ray, dist, surfel, refractive_index))
		return Radiance3(0.0);
    	
	depth += 1;

	const Vector3& w_o = -1 * ray.direction;

    // This point could be an emitter
    if (is_eye_ray && m_emit)
        l_o += surfel.material.emit;
    
    // Shade this point (direct illumination)
    if (!is_eye_ray || m_direct)
        l_o += estimate_direct_light_from_area_lights(random_seq, surfel, w_o,
			refractive_index);
    
    if (!is_eye_ray || m_indirect)
		l_o += estimate_indirect_light(random_seq, surfel, w_o, is_eye_ray);
    
	depth -= 1;

	return l_o;
}

Radiance3 Path_Tracer::estimate_direct_light_from_area_lights(
	random::Random_Sequence& random_seq,
	const scene::Surface_Element& surfel,
	const Vector3& w_o,
	double current_refractive_index)
{
	if (surfel.material.lambertian_reflect.r == 0 && depth >= 2 && dot_prod(w_o, surfel.geometric.normal) < 0)
		int a = 0;

    Radiance3 l_o(0.0);

    // Estimate radiance back along ray due to direct illumination from area lights
    const scene::Area_Light& light = pick_random_light();

	Radiance3 sample_power = light.m_power;
	Point3 sample_position(0.0);
	Vector3 sample_normal(0.0);
	light.sample_point(sample_position, sample_normal);

    // Displace surface points slightly along the triangle normals
    const Point3& surface_point_position = surfel.geometric.position + 
        surfel.geometric.normal * 1e-4;
    const Point3& light_surfel_position = sample_position + 
        sample_normal * 1e-4;

	// Points outwards
    Vector3& w_i = Vector3(surfel.geometric.position, light_surfel_position);
    double distance = w_i.magnitude();
    w_i /= distance;

    const Ray shadow_ray(surface_point_position, w_i);

    scene::Surface_Element shadow_ray_surfel;
	bool in_shadow = m_scene->intersect(
		shadow_ray, distance, shadow_ray_surfel, current_refractive_index);

	if (in_shadow && shadow_ray_surfel.material.emit != Irradiance3(0.0))
	{
		sample_power = shadow_ray_surfel.material.emit;
		sample_position = shadow_ray_surfel.geometric.position;
		sample_normal = shadow_ray_surfel.geometric.normal;
		distance = Vector3(surfel.geometric.position, sample_position).magnitude();
		in_shadow = false;
	}

    if (!in_shadow)
    {
        double geometric_term = 
			std::max(0.0, dot_prod(w_i, surfel.shading.normal)) 
			* std::max(0.0, dot_prod(-1 * w_i, sample_normal) / (distance * distance));

		static const double inv_pi = 1.0 / M_PI;

		const Radiance3& emitted_radiance = sample_power * inv_pi;
		const Radiance3& bsdf_evaluation = surfel.evaluate_bsdf(w_i, w_o);
		
		// Add surface color
		l_o += 
			m_scene->m_total_light_area    // Integration domain
			* emitted_radiance
			* bsdf_evaluation
			* geometric_term;    // Change of variables term
    }

    return l_o;
}

Radiance3 Path_Tracer::estimate_indirect_light(
	random::Random_Sequence& random_seq,
    const scene::Surface_Element& surfel,
	const Vector3& w_o,
	bool is_eye_ray)
{
    /*  Use recursion to estimate light running back along ray from surfel, but ONLY light that
        arrives from INDIRECT sources, by making a single-sample estimate of the arriving light */

    Radiance3 l_o(0.0);
    Vector3 scattered_direction(0);
    Color3 coeff(0);
    
	/*	Scatter "backwards", finding the direction of incoming light (scattered_direction) given the direction
		of outgoing light (ray.direction) */
	if (surfel.scatter(-1 * w_o, scattered_direction, coeff, random_seq))
	{
		double dot_product_val = dot_prod(surfel.geometric.normal, scattered_direction);
		double sign = (0.0 < dot_product_val) - (dot_product_val < 0.0);
		
		double outgoing_refractive_index = (sign > 0)
			? surfel.material.refractive_index_exterior
			: surfel.material.refractive_index_interior;
		
		const Point3& bumped_position = surfel.geometric.position + 
            (sign * 1e-4) * surfel.geometric.normal;
		
		Ray outgoing_ray(bumped_position, scattered_direction);
		l_o += coeff * path_trace(outgoing_ray, random_seq, false, outgoing_refractive_index);
    }
	
	return l_o;
}

const scene::Area_Light& Path_Tracer::pick_random_light()
{
	double inv_total_light_area = 1.0 / m_scene->m_total_light_area;

	std::uniform_real_distribution<double> dist(0.0, 1.0);
	double random_number = dist(random::mt_engine_singleton());

	for (const scene::Area_Light* area_light : m_scene->m_area_lights)
	{
		random_number -= area_light->area() * inv_total_light_area;
		if (random_number <= 0)
			return *area_light;
	}

	return *m_scene->m_area_lights.back();
}

void Path_Tracer::thread_code(std::vector<std::vector<Radiance3>>* image)
{
	const int resolution_height = (int) round(m_resolution_width / m_aspect_ratio);
	const double window_height = round(m_window_width / m_aspect_ratio);
	const double pixel_side = m_window_width / m_resolution_width;
	const Point3& near_center = m_camera->position() + (m_camera->near() * m_camera->look_at());
	const Point3& top_left_pixel_center = near_center
		+ 0.5 * (window_height - pixel_side) * m_camera->up()
		+ 0.5 * (m_window_width - pixel_side) * m_camera->left();
	const Vector3& right_increment = pixel_side * -1 * m_camera->left();
	const Vector3& down_increment = pixel_side * -1 * m_camera->up();
	m_pixel_lock.lock();

	while (m_current_row < resolution_height)
	{
		while (m_current_col < m_resolution_width && m_current_row < resolution_height)
		{
			int row = m_current_row;
			int col = m_current_col++;

			m_pixel_lock.unlock();

			Point3& pixel_center = top_left_pixel_center
				+ row * down_increment
				+ col * right_increment;
			const Ray ray(m_camera->position(), Vector3(m_camera->position(), pixel_center));
			random::Uniform_Random_Sequence random_seq;

			Radiance3 sample_estimate_sum = 0;
			for (int i = 0; i < m_samples_per_pixel; ++i)
				sample_estimate_sum += path_trace(ray, random_seq, true);

			m_image_lock.lock();
			const Radiance3& estimate = gamma_correction(sample_estimate_sum / m_samples_per_pixel);
			(*image)[row][col] = estimate;
			m_image_lock.unlock();

			m_pixel_lock.lock();

			if (PRINT_PROGRESS
				&& col == m_resolution_width - 1
				&& (row % (resolution_height / 20) == 0 || row == resolution_height-1))
			{
				int completed_pixels = (row + 1) * m_resolution_width;
				double progress_ratio = (double) completed_pixels / (resolution_height * m_resolution_width);
				std::cout << '\r' << concurrent_compute_image(progress_ratio);
			}
		}

		++m_current_row;
		m_current_col = 0;
	}

	m_pixel_lock.unlock();
}

std::string Path_Tracer::concurrent_compute_image(double progress) const
{
	if (progress < 0 || progress > 1)
		throw std::invalid_argument("Out of range");

	std::string concurrent_compute_image(20, ' ');
	std::string completed(int(progress / 0.05), '#');
	std::copy(completed.begin(), completed.end(), concurrent_compute_image.begin());
	
	return "0% [" + concurrent_compute_image + "] 100%";
}

Radiance3 Path_Tracer::gamma_correction(Radiance3 radiance) const
{
	return Radiance3(
		gamma_correction(radiance.r),
		gamma_correction(radiance.g),
		gamma_correction(radiance.b));
}

double Path_Tracer::gamma_correction(double radiance) const
{
	return int(
		std::pow(
			std::min(1.0, std::max(0.0, radiance * m_gamma_coefficient)),
			m_gamma_exponent)
		* 255.0);
}
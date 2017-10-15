#ifndef __GUARD_PATH_TRACER_H__
#define __GUARD_PATH_TRACER_H__

#include <mutex>
#include <stdexcept>
#include <vector>

#include "geometry/ray.h"
#include "geometry/vector3.h"
#include "path-tracer/camera.h"
#include "path-tracer/random_sequence.h"
#include "scene/area_light.h"
#include "scene/scene.h"
#include "shading/color3.h"
#include "shading/surface_element.h"

class Path_Tracer {
public:
	static const bool m_indirect = true;
	static const bool m_emit = true;
	static const bool m_direct = true;

	Path_Tracer(
		const Camera* camera,
		const scene::Scene* scene,
		double window_width,
		double aspect_ratio,
		int resolution_width,
		int samples_per_pixel,
		int num_threads = 4);

	void compute_image(std::vector<std::vector<Radiance3>>& image);

	// Acessor functions
	const Camera* camera() const { return m_camera; }
	const scene::Scene* scene() const { return m_scene; }
	double window_width() const { return m_window_width; }
	int resolution_width() const { return m_resolution_width; }
	double aspect_ratio() const { return m_aspect_ratio; }
	int samples_per_pixel() const { return m_samples_per_pixel; }
	int num_threads() const { return m_num_threads; }
	
	// Setters
	void set_camera(const Camera* camera);
	void set_scene(const scene::Scene* scene);
	void set_window_width(double window_width);
	void set_resolution_width(int resolution_width);
	void set_aspect_ratio(double aspect_ratio);
	void set_samples_per_pixel(int samples_per_pixel);
	void set_num_threads(int num_threads);

private:
	// Image-related members
	const Camera* m_camera;
	const scene::Scene* m_scene;
	double m_window_width;
	double m_aspect_ratio;
	int m_resolution_width;
	int m_samples_per_pixel;
	// Concurrency-related members
	std::mutex m_pixel_lock;
	std::mutex m_image_lock;
	int m_current_row;
	int m_current_col;
	int m_num_threads;

	Radiance3 path_trace(
		const Ray& ray,
		Random_Sequence& random_seq,
		bool is_eye_ray,
		double refractive_index = 1.0);
	
	Radiance3 estimate_direct_light_from_area_lights(
		Random_Sequence& random_seq,
		const scene::Surface_Element& surfel,
		const Vector3& w_o,
		double current_refractive_index);

	Radiance3 estimate_indirect_light(
		Random_Sequence& random_seq,
		const scene::Surface_Element& surfel,
		const Vector3& w_o,
		bool is_eye_ray);

	const scene::Area_Light& pick_random_light();

	void thread_code(std::vector<std::vector<Radiance3>>* image);

	std::string Path_Tracer::concurrent_compute_image(double progress) const;
};

#endif
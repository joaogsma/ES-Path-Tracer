#ifndef __GUARD_MONTE_CARLO_PATH_TRACER_H__
#define __GUARD_MONTE_CARLO_PATH_TRACER_H__

#include "../geometry/ray.h"
#include "../scene/scene.h"
#include "camera.h"
#include "path_tracer.h"
#include "scene/scene.h"
#include "shading/color3.h"

class Monte_Carlo_Path_Tracer : public Path_Tracer {
public:
	Monte_Carlo_Path_Tracer(
		const Camera* camera,
		const scene::Scene* scene,
		double window_width,
		double aspect_ratio,
		int resolution_width,
		int samples_per_pixel,
		double gamma_coefficient = 7,
		double gamma_exponent = 1.0 / 2.2,
		int num_threads = 4);

	int samples_per_pixel() const { return m_samples_per_pixel; }
	void set_samples_per_pixel(int samples_per_pixel);

private:
	int m_samples_per_pixel;

	Radiance3 estimate_pixel_color(const Ray& ray) const;
};

#endif
#ifndef ES_PATH_TRACER__PATH_TRACER__EVOLUTION_STRATEGY_PATH_TRACER_H_
#define ES_PATH_TRACER__PATH_TRACER__EVOLUTION_STRATEGY_PATH_TRACER_H_

#include "../geometry/ray.h"
#include "../scene/scene.h"
#include "camera.h"
#include "color_histogram.h"  // test
#include "path_tracer.h"
#include "scene/scene.h"
#include "shading/color3.h"

class Evolution_Strategy_Path_Tracer : public Path_Tracer {
public:
	Evolution_Strategy_Path_Tracer(
		const Camera* camera,
		const scene::Scene* scene,
		double window_width,
		double aspect_ratio,
		int resolution_width,
		int max_iterations,
		double children_population_ratio,
		int population_size,
		double gamma_coefficient = 7,
		double gamma_exponent = 1.0 / 2.2,
		int num_threads = 4);

	int max_iterations_per_pixel() const { return m_max_iterations_per_pixel; }
	double children_population_ratio() const { return m_children_population_ratio; }
	int population_size() const { return m_population_size; }

	void set_max_iterations_per_pixel(int max_iterations);
	void set_children_population_ratio(double children_population_ratio);
	void set_population_size(int population_size);

private:
	int m_max_iterations_per_pixel;
	double m_children_population_ratio;
	int m_population_size;

	virtual Radiance3 estimate_pixel_color(const Ray& ray) const;

	static bool color_compare_predicate(
		int color0,
		int color1,
		const Color_Histogram& color_histogram);
};

#endif

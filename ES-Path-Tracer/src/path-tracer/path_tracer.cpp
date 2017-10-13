#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

#include "common/random_number_engine.h"
#include "geometry/ray.h"
#include "path-tracer/path_tracer.h"
#include "path-tracer/random_sequence.h"
#include "scene/area_light.h"
#include "scene/scene.h"
#include "shading/color3.h"
#include "shading/surface_element.h"

class Path_Tracer {
public:
    const bool m_indirect = true;
    const bool m_emit = true;
    const bool m_direct = true;
    const bool m_point_lights = true;
    const bool m_area_lights = true;
    const bool m_direct_s = true;

	Radiance3 path_trace(
		const Ray& ray,
		const scene::Scene& scene,
		Random_Sequence& rnd,
		bool is_eye_ray,
		double refractive_index = 1)
    {
        /*  Compute the radiance BACK along the given ray. In the event that the ray is an eye-ray, 
            include light emitted by the first surface encountered. For subsequent rays, such light 
            has already been counted in the computation of direct lighting at prior hits */

        Radiance3 l_o(0.0);

        scene::Surface_Element surfel;

        double dist = std::numeric_limits<double>::infinity();

        if (scene.intersect(ray, dist, surfel, refractive_index))
        {
			const Vector3& w_o = -1 * ray.direction;

            // This point could be an emitter
            if (is_eye_ray && m_emit)
                l_o += surfel.material.emit;
        
            // Shade this point (direct illumination)
            if (!is_eye_ray || m_direct)
            {
                l_o += estimate_direct_light_from_area_lights(scene, rnd, surfel, w_o,
					refractive_index);
            }
        
            if (!is_eye_ray || m_indirect)
                l_o += estimate_indirect_light(scene, rnd, surfel, w_o, is_eye_ray);
        }

        return l_o;
    }

private:
    Radiance3 estimate_direct_light_from_area_lights(
		const scene::Scene& scene,
		Random_Sequence& rnd,
		const scene::Surface_Element& surfel,
		const Vector3& w_o,
		double current_refractive_index)
    {
        Radiance3 l_o(0.0);

        if (m_area_lights)
        {
			// Estimate radiance back along ray due to direct illumination from area lights
            const scene::Area_Light& light = pick_random_light(scene);

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
			bool in_shadow = scene.intersect(
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
				for (int color_channel = 0; color_channel < 3; ++color_channel)
				{
					l_o[color_channel] += 
						scene.m_total_light_area    // Integration domain
						* emitted_radiance[color_channel]
						* bsdf_evaluation[color_channel]
						* geometric_term;    // Change of variables term
				}
            }
        }

        return l_o;
    }

    Radiance3 estimate_indirect_light(
		const scene::Scene& scene,
		Random_Sequence& rnd,
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
		if ((!is_eye_ray || m_indirect)
			&& surfel.scatter(-1 * w_o, scattered_direction, coeff, rnd))
		{
			double dot_product_val = dot_prod(surfel.geometric.normal, scattered_direction);
			double sign = (0.0 < dot_product_val) - (dot_product_val < 0.0);
			
			double outgoing_refractive_index = (sign > 0)
				? surfel.material.refractive_index_exterior
				: surfel.material.refractive_index_interior;
			
			const Point3& bumped_position = surfel.geometric.position + 
                (sign * 1e-4) * surfel.geometric.normal;
			
			Ray outgoing_ray(bumped_position, scattered_direction);
			l_o += coeff * path_trace(outgoing_ray, scene, rnd, false, outgoing_refractive_index);
        }

        return l_o;
    }

	const scene::Area_Light& pick_random_light(const scene::Scene& scene)
	{
		double inv_total_light_area = 1.0 / scene.m_total_light_area;

		std::uniform_real_distribution<double> dist(0.0, 1.0);
		double random_number = dist(random::mt_engine_singleton());

		for (const scene::Area_Light* area_light : scene.m_area_lights)
		{
			random_number -= area_light->area() * inv_total_light_area;
			if (random_number <= 0)
				return *area_light;
		}

		return *scene.m_area_lights.back();
	}
};
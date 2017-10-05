#define _USE_MATH_DEFINES

#include <algorithm>
#include <numeric>
#include <cmath>

#include "shading/color3.h"
#include "geometry/ray.h"
#include "path-tracer/path_tracer.h"
#include "path-tracer/random_sequence.h"
#include "scene/area_light.h"
#include "scene/point_light.h"
#include "scene/scene.h"
#include "shading/surface_element.h"

class Path_Tracer {
    const bool m_indirect = true;
    const bool m_emit = true;
    const bool m_direct = true;
    const bool m_point_lights = true;
    const bool m_area_lights = true;
    const bool m_direct_s = true;

    Radiance3 path_trace(const Ray& ray, const scene::Scene& scene, Random_Sequence& rnd, bool is_eye_ray)
    {
        /*  Compute the radiance BACK along the given ray. In the event that the ray is an eye-ray, 
            include light emitted by the first surface encountered. For subsequent rays, such light 
            has already been counted in the computation of direct lighting at prior hits */

        Radiance3 l_o(0.0);

        scene::Surface_Element surfel;

        double dist = std::numeric_limits<double>::infinity();

        if (scene.intersect(ray, dist, surfel))
        {
            // This point could be an emitter
            if (is_eye_ray && m_emit)
                l_o += surfel.material.emit;
        
            // Shade this point (direct illumination)
            if (!is_eye_ray || m_direct)
            {
                //l_o += estimate_direct_light_from_point_lights(scene, surfel, ray);
                l_o += estimate_direct_light_from_area_lights(scene, rnd, surfel, ray);
            }
        
            if (!is_eye_ray || m_indirect)
                l_o += estimate_indirect_light(scene, rnd, surfel, ray, is_eye_ray);
        
        }

        return l_o;
    }

	/*
    Radiance3 estimate_direct_light_from_point_lights(const scene::Scene& scene, 
        const scene::Surface_Element& surfel, const Ray& ray)
    {
        Radiance3 l_o(0.0);

        if (m_point_lights)
        {
            for (const scene::Point_Light* ptr : scene.m_point_lights)
            {
                const scene::Point_Light& light = *ptr;

                // Displace surface point slightly along the triangle normal
                const Point3& position = surfel.geometric.position + surfel.geometric.normal * 1e-4;

                const Vector3& w_i = Vector3(surfel.geometric.position, light.position);
                double distance = w_i.magnitude();
                w_i /= distance;
                
                const Ray shadow_ray(position, w_i);

                scene::Surface_Element dummy_surfel;

                if ( !scene.intersect(shadow_ray, distance, dummy_surfel) )    // Check if the point is in shadow
                {
                    // Attenuated radiance
                    const Irradiance3& e_i = light.power / (4.0 * M_PI * distance * distance);

                    l_o += surfel.evaluate_bsdf( w_i, -1 * ray.direction ) * e_i *
                        std::max( 0.0, dot_prod(w_i, surfel.shading.normal) );
                }
            }
        }

        return l_o;
    }
	*/

    Radiance3 estimate_direct_light_from_area_lights(const scene::Scene& scene, 
        Random_Sequence& rnd, const scene::Surface_Element& surfel, const Ray& ray)
    {
        Radiance3 l_o(0.0);

        if (m_area_lights)
        {
            // Estimate radiance back along ray due to direct illumination from area lights
            for (const scene::Area_Light* ptr : scene.m_area_lights)
            {
                const scene::Area_Light& light = *ptr;

                const scene::Surface_Element& light_surfel = light.sample_point(rnd);

                // Displace surface points slightly along the triangle normals
                const Point3& surface_point_position = surfel.geometric.position + 
                    surfel.geometric.normal * 1e-4;
                const Point3& light_surfel_position = light_surfel.geometric.position + 
                    light_surfel.geometric.normal * 1e-4;

                const Vector3& w_i = Vector3(surfel.geometric.position, 
                    light_surfel.geometric.position);
                double distance = w_i.magnitude();
                w_i /= distance;

                const Ray shadow_ray(surface_point_position, w_i);

                scene::Surface_Element dummy_surfel;

                if (!scene.intersect(shadow_ray, distance, dummy_surfel))
                {
                    double geometric_term = std::max( 0.0, dot_prod(w_i, surfel.shading.normal) ) *
                        std::max( 0.0, dot_prod(-1 * w_i, light_surfel.geometric.normal) / (distance * distance) );

					// Add surface color
                    l_o += surfel.evaluate_bsdf(w_i, -1 * ray.direction) * 
						(light.power / M_PI) * geometric_term;
                }
            }

			/*
            // Now add in impulse-reflected light as well
            if (m_direct_s)
            {
                std::vector<scene::Surface_Element::Light_Impulse> impulses;
                surfel.get_bsdf_impulses(-1 * ray.direction, impulses);

                for (const scene::Surface_Element::Light_Impulse& impulse : impulses)
                {
                    const Point3& bumped_position = surfel.geometric.position + impulse.direction * 1e-4;
                    const Ray secondary_ray(bumped_position, impulse.direction);

                    scene::Surface_Element surfel2;
                    double distance = std::numeric_limits<double>::infinity();

                    if ( m_emit && scene.intersect(secondary_ray, distance, surfel2) )
                        l_o += surfel2.material.emit * impulse.magnitude;
                }
            }
			*/
        }

        return l_o;
    }

    Radiance3 estimate_indirect_light(const scene::Scene& scene, Random_Sequence& rnd, 
        const scene::Surface_Element& surfel, const Ray& ray, bool is_eye_ray)
    {
        Radiance3 l_o(0.0);

        /*  Use recursion to estimate light running back along ray from surfel, but ONLY light that
            arrives from INDIRECT sources, by making a single-sample estimate of the arriving light */

        Vector3& w_o = -1 * ray.direction;
        Vector3 w_i(0);
        Color3 coeff(0);
        double eta_o = 0.f;

        if ( (!is_eye_ray || m_indirect) && 
            surfel.scatter(w_i, w_o, coeff, eta_o, rnd) )
        {
            float eta_i = surfel.material.eta_reflect;
            float refractive_scale = std::pow(eta_i / eta_o, 2);

            double dot_product_val = dot_prod(surfel.geometric.normal, w_o);
            double sign = (0.0 < dot_product_val) - (dot_product_val < 0.0);

            const Point3& bumped_position = surfel.geometric.position + 
                (sign * 1e-4 * surfel.geometric.normal);

            l_o += refractive_scale * coeff * 
				path_trace(Ray(bumped_position, w_o), scene, rnd, false);
        }

        return l_o;
    }
};
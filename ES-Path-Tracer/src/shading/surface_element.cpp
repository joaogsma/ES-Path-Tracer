#define _USE_MATH_DEFINES

#include <cmath>
#include <algorithm>
#include <limits>

#include "shading/color3.h"
#include "geometry/vector3.h"
#include "geometry/point3.h"
#include "path-tracer/random_sequence.h"
#include "shading/surface_element.h"

namespace scene
{
    bool Surface_Element::scatter(const Vector3& w_i, Vector3& w_o, Color3& coefficient, 
		Random_Sequence& rnd) const
    {
        const Vector3& n = geometric.normal;

        /*  Choose a next number on [0, 1], then reduce it by each kind of 
            scatering's probability until it becomes negative */
        double r = rnd.next();
        
		static const Color3& zero_color = Color3::zero();
		static const Vector3& zero_vector = Vector3(0.0);

        // ========== Lambertian scattering ==========
        if (material.lambertian_reflect != zero_color)
        {
            double prob_lambertian_avg = (material.lambertian_reflect[0]
				+ material.lambertian_reflect[1] + material.lambertian_reflect[2]) / 3.0;
            r -= prob_lambertian_avg;
            
            if (r < 0.0)
            {
				double prob_times_density = prob_lambertian_avg * (1.0 / (2 * M_PI));
                w_o = rnd.uniform_distributed_hemisphere_sample(geometric);
                coefficient = material.lambertian_reflect / prob_times_density;
                return true;
            }
        }
        // ===========================================


        // ========== Specular scattering ==========
        if (material.specular_reflect != zero_color)
        {
            const double prob_specular_avg = (material.specular_reflect[0] 
					+ material.specular_reflect[1] + material.specular_reflect[2]) / 3.0;
            r -= prob_specular_avg;

            if (r < 0.0)
            {
                w_o = mirror_reflect(w_i);
				coefficient = material.specular_reflect / prob_specular_avg;
                return true;
            }
        }
        // =======================================


        // ========== Transmissive scattering ==========
        if (material.transmit != zero_color)
        {
            const double prob_transmit_avg = (material.transmit[0] + material.transmit[1]
				+ material.transmit[2]) / 3.0;
            r -= prob_transmit_avg;

            if (r < 0.0)
            {
                coefficient = material.transmit / prob_transmit_avg;
                w_o = refract(w_i);
                return w_o != zero_vector;    // w_o is zero on total internal refraction
            }
        }
        // =============================================

        // Absorbed
        return false;
    }

	// Assumes incoming is pointing inwards
	Vector3 Surface_Element::mirror_reflect(const Vector3& incoming) const
	{
		Vector3 normal = geometric.normal;
		double cosine = dot_prod(incoming, -1 * geometric.normal);
		
		// Light is coming from the object's interior - flip the normal and restart
		if (cosine < 0.0)
		{
			normal *= -1;
			cosine = dot_prod(incoming, geometric.normal);
		}

		// Flip the normal vector if light is coming from the object's interior
		return incoming + 2 * cosine * normal;
	}

	Vector3 Surface_Element::refract(const Vector3& incoming) const
	{
		Vector3 normal = geometric.normal;
		double cos_i = dot_prod(incoming, -1 * geometric.normal);
		double refractive_index_i = material.refractive_index_exterior;
		double refractive_index_o = material.refractive_index_interior;
		
		// Light is coming from the object's interior - flip the normal and restart
		if (cos_i < 0.0)
		{
			normal *= -1;
			double cos_i = dot_prod(incoming, geometric.normal);
			std::swap(refractive_index_i, refractive_index_o);
		}

		double ratio = refractive_index_i / refractive_index_o;
		double sin_o = ratio * sqrt(1 - cos_i * cos_i);

		if (sin_o > 1.0 - 1e-4)    // Critical angle - ray is absorbed
			return Vector3(0.0);

		double cos_o = sqrt(1 - sin_o * sin_o);
		return (incoming * ratio) + (ratio * cos_i - cos_o) * normal;
	}
}
#ifndef __GUARD_SURFACE_ELEMENT_H__
#define __GUARD_SURFACE_ELEMENT_H__

#include <stdexcept>

#include "shading/color3.h"
#include "geometry/vector3.h"
#include "geometry/point3.h"
#include "path-tracer/random_sequence.h"

namespace scene
{
    class Surface_Element {
    public:    
        // ========== Data structures ==========
        struct Shading_Data {
            /*  The normal is the interpolated vertex normal to be used during shading. */
            Vector3 normal;

            Shading_Data() : normal(0) {}
            Shading_Data(const Vector3& normal) : normal(normal) {}
        };

        struct Geometric_Data {
            /*  The normal is the triangle's face normal, which can be used for ray bumping; 
			the tangent0 and tangent1 vectors form an orthonormal basis with it. The position 
			is the actual position on the surface, and may be changed by displacement or bump 
			mapping */

            Vector3 normal, tangent0, tangent1;
            Point3 position;

            Geometric_Data() : normal(0), tangent0(0), tangent1(0), position(0) {}
            Geometric_Data(const Vector3& normal, const Vector3& tangent0, const Vector3& tangent1,
				const Point3& position)
				: normal(normal), tangent0(tangent0), tangent1(tangent1), position(position) {}
        };

        struct Material_Data {
		public:
			Irradiance3 emit;
            Color3 lambertian_reflect, specular_reflect, transmit;
            double refractive_index_exterior, refractive_index_interior, glossy_exponent;

            Material_Data() : emit(0), lambertian_reflect(0), specular_reflect(0), transmit(0), 
                refractive_index_exterior(0), refractive_index_interior(0), glossy_exponent(0) {}
            
            Material_Data(const Irradiance3& emit, const Color3& lambertian_reflect,
                const Color3& specular_reflect, const Color3& transmit, double glossy_exponent,
				double refractive_index_exterior, double refractive_index_interior)
                : emit(emit), lambertian_reflect(lambertian_reflect), specular_reflect(specular_reflect), 
                transmit(transmit), refractive_index_exterior(refractive_index_exterior), 
				refractive_index_interior(refractive_index_interior), glossy_exponent(glossy_exponent)
            {
                if (!valid())
                    throw std::domain_error("Invalid_parameters");
            }

		private:
				bool valid()
            {
                bool valid = true;
                
                for (int i = 0; i < 3; ++i)
                {
                    valid = valid && emit[i] >= 0 &&
                        lambertian_reflect[i] >= 0.0 && lambertian_reflect[i] <= 1.0 &&
                        specular_reflect[i] >= 0.0 && specular_reflect[i] <= 1.0 &&
                        transmit[i] >= 0.0 && transmit[i] <= 1.0;
                }

                valid = valid && refractive_index_interior >= 0.0 && glossy_exponent >= 0.0;

                for (int i = 0; i < 3; ++i)
                    valid = valid && (lambertian_reflect[i] + specular_reflect[i] + transmit[i] <= 1.0);

                return valid;
            }
        };
        // =====================================
        
        // ========== Member variables ==========
        Shading_Data shading;
        Geometric_Data geometric;
        Material_Data material;
        // ======================================

        bool scatter(
			const Vector3& w_i,
			Vector3& w_o,
			Color3& coefficient,
			Random_Sequence& rnd) const;

		// Assumes both vectors point outwards
		Radiance3 evaluate_bsdf(const Vector3& w_i, const Vector3& w_o) const;

	private:
		Vector3 mirror_reflect(const Vector3& incident, Vector3 normal = Vector3(0.0)) const;
		Vector3 refract(const Vector3& incoming, Vector3 normal = Vector3(0.0)) const;
    };
};

#endif

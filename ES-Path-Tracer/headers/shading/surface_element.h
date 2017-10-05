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
            Color3 lambertian_reflect, glossy_reflect, transmit;
            double eta_reflect, eta_transmit, glossy_exponent;

            Material_Data() : emit(0), lambertian_reflect(0), glossy_reflect(0), transmit(0), 
                eta_reflect(0), eta_transmit(0), glossy_exponent(0) {}
            
            Material_Data(const Irradiance3& emit, const Color3& lambertian_reflect,
                const Color3& glossy_reflect, const Color3& transmit, double glossy_exponent,
                double eta_reflect, double eta_transmit)
                : emit(emit), lambertian_reflect(lambertian_reflect), glossy_reflect(glossy_reflect), 
                transmit(transmit), eta_reflect(eta_reflect), eta_transmit(eta_transmit), 
                glossy_exponent(glossy_exponent)
            {
                if ( !valid() )
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
                        glossy_reflect[i] >= 0.0 && glossy_reflect[i] <= 1.0 &&
                        transmit[i] >= 0.0 && transmit[i] <= 1.0;
                }

                valid = valid && eta_reflect >= 0.0 && eta_transmit >= 0.0 && glossy_exponent >= 0.0;

                for (int i = 0; i < 3; ++i)
                    valid = valid && (lambertian_reflect[i] + glossy_reflect[i] + transmit[i] <= 1.0);

                return valid;
            }
        };
        
		/*
        struct Light_Impulse {
            Vector3 direction;
            double magnitude;

            Light_Impulse() : direction(0), magnitude(0) {}
            Light_Impulse(const Vector3& direction, double magnitude)
                : direction(direction), magnitude(magnitude) {}
        };
		*/

        // =====================================
        
        // ========== Member variables ==========
        Shading_Data shading;
        Geometric_Data geometric;
        Material_Data material;
        // ======================================

        bool scatter(Vector3& w_i, const Vector3& w_o, Color3& weight_o, double& eta_o, 
			Random_Sequence& rnd) const;

	private:
		Vector3 reflect_about(const Vector3& incident) const;
    };
}

#endif

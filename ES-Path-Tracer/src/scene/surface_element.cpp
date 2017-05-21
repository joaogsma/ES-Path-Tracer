#include <algorithm>
#include <limits>

#include "color/color3.h"
#include "geometry/vector3.h"
#include "geometry/point3.h"
#include "path-tracer/random_sequence.h"
#include "scene/surface_element.h"

namespace scene
{
    bool Surface_Element::scatter(const Vector3& w_i, Vector3& w_o, Color3& weight_o,
        double& eta_o, /*Color3& extinction_o,*/ Random_Sequence& rnd) const
    {
        const Vector3& n = shading.normal;

        /*  Choose a next number on [0, 1], then reduce it by each kind of 
            scaterring's probability until it becomes negative */
        double r = rnd.next();

        const Color3& zero = Color3::zero();

        // ========== Lambertian scattering ==========
        if ( material.lambertian_reflect != zero )
        {
            double prob_lambertian_avg = (material.lambertian_reflect[0] + 
                material.lambertian_reflect[1] + material.lambertian_reflect[2]) / 3.0;

            r -= prob_lambertian_avg;
            
            if (r < 0.0)
            {
                weight_o = material.lambertian_reflect * (1.0 / prob_lambertian_avg);
                w_o = rnd.cos_hemisphere_random(n);
                eta_o = material.eta_reflect;
                //extinction_o = material.extinction_reflect;

                return true;
            }
        }
        // ===========================================


        // ========== Glossy scattering ==========
        Color3 f(0, 0, 0); // Fresnel term
        bool f_init = false;

        if ( material.glossy_reflect != zero )
        {
            // Cosine of the angle of incidence, for computing the Fresnel term
            const double cos_i = std::max( 1e-3, dot_prod(w_i, n) );
            f = compute_fresnel_term(material.glossy_reflect, w_i);
            f_init = true;

            const Color3& prob_specular = f;
            const double prob_specular_avg = (prob_specular[0] + prob_specular[1] + 
                prob_specular[2]) / 3.0;

            r -= prob_specular_avg;
            if (r < 0.0)
            {
                if (material.glossy_exponent != std::numeric_limits<double>::infinity())    // Glossy (non-mirror) case
                {
                    double intensity = glossy_scatter(w_i, material.glossy_exponent, rnd, w_o) / 
                        prob_specular_avg;

                    if (intensity < 0.0)    // Absorb
                        return false;

                    weight_o = prob_specular * intensity;
                }
                else    // Mirror
                {
                    w_o = reflect_about(w_i, n);
                    weight_o = prob_specular * (1.0 / prob_specular_avg);
                }

                eta_o = material_eta_reflect;
                //extinction_o = material.extinction_reflect;
                
                return true;
            }
        }
        // =======================================


        // ========== Transmissive scattering ==========
        if ( material.transmit != zero )
        {
            // Fresnel transmissive coefficient
            Color3 f_t(0.0);

            if (f_init)
                f_t = Color3(1.0) - f;
            else
            {
                // Cosine of the angle of incidence, for computing F
                const double cos_i = std::max(1e-3, dot_prod(w_i, n));
                // Schlik approximation
                f_t = Color3( 1.0 - std::pow(1.0 - cos_i, 5) );
            }

            const Color3& t0 = material.transmit;
            const Color3& prob_transmit = f_t * t0;
            
            const double prob_transmit_avg = (prob_transmit[0] + prob_transmit[1] + 
                prob_transmit[2]) / 3.0;

            r -= prob_transmit_avg;
            if (r < 0.0)
            {
                weight_o = prob_transmit * (1.0 / prob_transmit_avg);
                w_o = refraction_direction(-1 * w_i, n, material.eta_transmit, material.eta_reflect);
                eta_o = material.eta_transmit;
                //extinction_o = material.extinction_transmit;

                // w_o is zero  on total internal refraction
                return w_o != Vector3(0.0);
            }
        }
        // =============================================

        // Absorbed
        return false;
    }
}
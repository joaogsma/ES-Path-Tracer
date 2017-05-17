#ifndef __GUARD_SURFACE_ELEMENT_H__
#define __GUARD_SURFACE_ELEMENT_H__

#include "geometry/vector3.h"
#include "geometry/point3.h"

namespace scene
{
    class Surface_Element {
    public:    // Data structures
        struct Shading_Data {
            /*  The normal is the interpolated vertex normal to be used during shading.
            The tangent vectors are the u and v basis of the tangent plane, and form
            an orthonormal basis with the normal. */

            Vector3 normal;
            Vector3 tangent0;
            Vector3 tangent1;

            Shading_Data() : normal(0), tangent0(0), tangent1(0) {}
            Shading_Data(const Vector3& normal, const Vector3& tangent0, const Vector3& tangent1)
                : normal(normal), tangent0(tangent0), tangent1(tangent1) {}
        };

        struct Geometric_Data {
            /*  The normal is the triangle's face normal. It can be used for ray bumping.
            The position is the actual position on the surface, and may be changed by
            displacement or bump mapping */

            Vector3 normal;
            Point3 position;

            Geometric_Data() : normal(0), position(0) {}
            Geometric_Data(const Vector3& normal, const Point3& position)
                : normal(normal), position(position) {}
        };

        struct Light_Impulse {
            Vector3 direction;
            double magnitude;

            Light_Impulse() : direction(0), magnitude(0) {}
            Light_Impulse(const Vector3& direction, double magnitude)
                : direction(direction), magnitude(magnitude) {}
        };

        // Member variables
        Shading_Data shading;
        Geometric_Data geometric;
    };
}

#endif

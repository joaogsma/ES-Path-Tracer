#ifndef __GUARD_POINT_LIGHT_H__
#define __GUARD_POINT_LIGHT_H__

#include "shading/color3.h"
#include "geometry/point3.h"
#include "light.h"

namespace scene
{
    class Point_Light : public Light {
    public:
        Point3 position;

        Point_Light(const Radiance3& power, const Point3& position);
    };
}

#endif

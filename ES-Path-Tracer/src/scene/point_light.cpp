#include "scene/point_light.h"

namespace scene
{
    Point_Light::Point_Light(const Radiance3& power, const Point3& position)
        : Light(power), position(position) {}
}
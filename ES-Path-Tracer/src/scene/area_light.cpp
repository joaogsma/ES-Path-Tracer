#include <vector>

#include "color/color3.h"
#include "geometry/triangle.h"
#include "scene/area_light.h"
#include "scene/light.h"

namespace scene
{
    Area_Light::Area_Light(const Radiance3& power, const std::vector<const Triangle*>& triangles)
        : Light(power), m_kd_tree(triangles) {}

    bool Area_Light::intersect(const Ray& ray, double& t, Surface_Element& surfel) const
    {
        const Triangle *hit_triangle = m_kd_tree.intersect(ray);

        // TODO
    }
}
#ifndef __GUARD_AREA_LIGHT_H__
#define __GUARD_AREA_LIGHT_H__

#include <vector>

#include "shading/color3.h"
#include "geometry/aab.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "kd-tree/kd_tree.h"
#include "light.h"
#include "shading/surface_element.h"

namespace scene
{
    class Area_Light : public Light {
    public:
        Area_Light(const Radiance3& power, const std::vector<const Triangle*>& triangles);

        bool intersect(const Ray& ray, double& t, Surface_Element& surfel) const;
        const AAB& aabb() const { return m_kd_tree.aabb(); }

    private:
        kd_tree::KD_Tree m_kd_tree;
    };
}

#endif

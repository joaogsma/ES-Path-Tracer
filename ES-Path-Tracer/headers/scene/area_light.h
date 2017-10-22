#ifndef __GUARD_AREA_LIGHT_H__
#define __GUARD_AREA_LIGHT_H__

#include <vector>

#include "../geometry/aab.h"
#include "../geometry/ray.h"
#include "../geometry/triangle.h"
#include "../kd-tree/kd_tree.h"
#include "../shading/color3.h"
#include "../shading/surface_element.h"
#include "light.h"

namespace scene
{
    class Area_Light : public Light {
    public:
        Area_Light(const Radiance3& m_power, const std::vector<const Triangle*>& triangles);

        bool intersect(const Ray& ray, double& t, Surface_Element& surfel) const;
        
		const AAB& aabb() const { return m_kd_tree.aabb(); }

		double area() const { return m_area; }

		void sample_point(Point3& sample_position, Vector3& sample_normal) const;

    private:
		const double m_area;
        const kd_tree::KD_Tree m_kd_tree;
		const std::vector<const Triangle*> m_triangles;

		static double total_area(const std::vector<const Triangle*>& triangles);

		const Triangle* sample_triangle() const;
    };
}

#endif

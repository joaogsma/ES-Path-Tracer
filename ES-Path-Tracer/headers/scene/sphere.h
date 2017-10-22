#ifndef __GUARD_QUADRIC_H__
#define __GUARD_QUADRIC_H__

#include "../geometry/aab.h"
#include "../geometry/ray.h"
#include "../geometry/triangle.h"
#include "../kd-tree/kd_tree.h"
#include "../shading/surface_element.h"
#include "object.h"

namespace scene
{
	class Sphere : public Object {
	public:
		Sphere(const Point3& center, double radius, const Surface_Element::Material_Data& material);

		// Acessor functions
		const Point3& center() const { return m_center; }
		double radius() const { return m_radius; }
		const AAB& aabb() const { return m_aabb; }
		// Setters
		void set_center(const Point3& center);
		void set_radius(double radius);
		
		bool intersect(const Ray& ray, double& t, Surface_Element& surfel) const;

	private:
		Point3 m_center;
		double m_radius;
		AAB m_aabb;

		int evaluate(const Point3& point);

		bool solve_quadratic(double a, double b, double c, double& x0, double& x1) const;

		void update_aabb();
	};
}

#endif

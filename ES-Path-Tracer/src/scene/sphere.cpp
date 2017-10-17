#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "geometry/point3.h"
#include "geometry/vector3.h"
#include "scene/sphere.h"

namespace scene
{
	Sphere::Sphere(const Point3& center, double radius, const Surface_Element::Material_Data& material)
		: Object(material), m_center(center), m_radius(radius)
	{
		update_aabb();
	}

	void Sphere::set_center(const Point3& center)
	{
		m_center = center;
		update_aabb();
	}

	void Sphere::set_radius(double radius)
	{
		if (radius <= 0)
			throw std::invalid_argument("Radius must be greater than 0");
		m_radius = radius;
		update_aabb();
	}

	bool Sphere::intersect(const Ray& ray, double& t, Surface_Element& surfel) const
	{
		const Vector3& origin_minus_center = ray.origin - m_center;
		double a = 1.0;
		double b = 2 * dot_prod(ray.direction, origin_minus_center);
		double c = dot_prod(origin_minus_center, origin_minus_center) - m_radius * m_radius;

		double t0, t1;

		if (!solve_quadratic(a, b, c, t0, t1))
			return false;    // No solutions

		if (std::max(t0, t1) < 0)
			return false;    // intersections for negative t only

		t = (t0 >= 0) ? t0 : t1;

		surfel.geometric.position = ray.origin + (t * ray.direction);
		const Vector3& normal = (surfel.geometric.position - m_center).normalize();
		surfel.geometric.normal = normal;
		surfel.shading.normal = normal;
		surfel.material = m_material;

		static const Vector3 unit_x(1, 0, 0);
		static const Vector3 unit_y(0, 1, 0);
		surfel.geometric.tangent0 = (1 - abs(dot_prod(normal, unit_x)) < 1e-4)
			? cross_prod(unit_x, normal)
			: cross_prod(unit_y, normal);
		surfel.geometric.tangent1 = cross_prod(normal, surfel.geometric.tangent0);
		return true;
	}

	int Sphere::evaluate(const Point3& point)
	{
		double dist2 = distance2(point, m_center);
		double radius2 = m_radius * m_radius;

		if (abs(dist2 - radius2) <= 1e-4)
			return 0;

		return dist2 > radius2 ? 1 : -1;
	}

	bool Sphere::solve_quadratic(double a, double b, double c, double& x0, double& x1) const
	{
		double discriminant = b * b - 4 * a * c;
		
		if (discriminant < 0)    // No solutions
			return false;

		if (discriminant == 0)    // One real solution
		{
			x0 = x1 = -0.5 * b / a;
		}
		else    // Two real solutions
		{
			double q = b > 0
				? -0.5 * (b + sqrt(discriminant))
				: -0.5 * (b - sqrt(discriminant));
			x0 = q / a;
			x1 = c / q;
		}

		if (x0 > x1)
			std::swap(x0, x1);

		return true;
	}

	void Sphere::update_aabb()
	{
		m_aabb = AAB(
			m_center.x - m_radius,
			m_center.x + m_radius,
			m_center.y - m_radius,
			m_center.y + m_radius,
			m_center.z - m_radius,
			m_center.z + m_radius);
	}
}
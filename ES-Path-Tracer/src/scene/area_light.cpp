#include <random>
#include <vector>

#include "common/random_number_engine.h"
#include "geometry/triangle.h"
#include "scene/area_light.h"
#include "scene/light.h"
#include "shading/color3.h"

namespace scene
{
    Area_Light::Area_Light(const Radiance3& m_power, const std::vector<const Triangle*>& triangles)
        : Light(m_power), m_area(total_area(triangles)), m_kd_tree(triangles), m_triangles(triangles) {}

	bool Area_Light::intersect(const Ray& ray, double& t, Surface_Element& surfel) const
    {
        const Triangle *triangle = m_kd_tree.intersect(ray);

		if (!triangle)
			return false;

		std::vector<double> bar_weights;
		if (!ray.intersect(*triangle, t, bar_weights))
			return false;

		surfel.geometric.normal = triangle->normal();
		surfel.geometric.position = ray.origin + t * ray.direction;
		surfel.material.emit = m_power;
		return true;
    }

	void Area_Light::sample_point(Point3& sample_position, Vector3& sample_normal) const
	{
		const Triangle& triangle = *sample_triangle();
		
		std::uniform_real_distribution<double> dist(0.0, 1.0);
		double alpha = dist(random::mt_engine_singleton());
		double beta = dist(random::mt_engine_singleton());
		double gamma = 1.0 - alpha - beta;

		sample_normal = triangle.normal();
		sample_position = Point3(
			(alpha * triangle.vertex(0)->x) + (beta * triangle.vertex(1)->x)
				+ (gamma * triangle.vertex(3)->x),
			(alpha * triangle.vertex(0)->y) + (beta * triangle.vertex(1)->y)
				+ (gamma * triangle.vertex(3)->y),
			(alpha * triangle.vertex(0)->z) + (beta * triangle.vertex(1)->z)
				+ (gamma * triangle.vertex(3)->z)
		);
	}

	double Area_Light::total_area(const std::vector<const Triangle*>& triangles)
	{
		double total_area = 0;

		for (const Triangle* triangle : triangles)
			total_area += triangle->area();

		return total_area;
	}

	const Triangle* Area_Light::sample_triangle() const
	{
		std::uniform_real_distribution<double> dist(0.0, 1.0);
		double random_number = dist(random::mt_engine_singleton());
		double inv_total_area = 1.0 / m_area;

		for (const Triangle* triangle : m_triangles)
		{
			random_number -= triangle->area() * inv_total_area;
			if (random_number <= 0.0)
				return triangle;
		}

		return m_triangles.back();
	}
}
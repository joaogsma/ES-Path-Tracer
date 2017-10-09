#include <vector>
#include <cfloat>

#include "shading/color3.h"
#include "geometry/vector3.h"
#include "scene/object.h"
#include "scene/scene.h"
#include "shading/surface_element.h"

using std::vector;

namespace scene
{
    Scene::~Scene()
    {
        for (const Object* ptr : m_objects)
            delete ptr;

        for (const Point_Light* ptr : m_point_lights)
            delete ptr;

        for (const Area_Light* ptr : m_area_lights)
            delete ptr;
    }

    bool Scene::intersect(const Ray& ray, double& t, Surface_Element& result,
		double refractive_index) const
    {
        bool found_intersection = false;

        for (const Object* obj : m_objects)
        {
            double hit_t;
            Surface_Element surfel;

            if (obj->intersect(ray, hit_t, surfel) && hit_t < t)
            {
                t = hit_t;
				surfel.material.refractive_index_exterior = refractive_index;
				result = surfel;
                found_intersection = true;
            }
        }

        return found_intersection;
    }
}
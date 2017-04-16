#include <vector>
#include <cfloat>

#include "color/color3.h"
#include "geometry/vector3.h"
#include "scene/object.h"
#include "scene/scene.h"

using std::vector;

namespace scene
{
    bool Scene::hit(const Ray &ray, Color3 &color, Vector3 &normal)
    {
        double best_t = DBL_MAX;

        for (vector<Object*>::const_iterator it = objects.begin(); it != objects.end(); ++it)
        {
            double hit_t;
            Color3 hit_color(0, 0, 0);
            Vector3 hit_normal(0, 0, 0);

            if ( (*it)->hit(ray, hit_t, hit_color, hit_normal) && hit_t < best_t )
            {
                best_t = hit_t;
                color = hit_color;
                normal = hit_normal;
            }
        }

        return best_t < DBL_MAX;
    }
}
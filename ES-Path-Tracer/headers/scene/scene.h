#ifndef __GUARD_SCENE_H__
#define __GUARD_SCENE_H__

#include <vector>

#include "area_light.h"
#include "shading/color3.h"
#include "geometry/vector3.h"
#include "object.h"
#include "point_light.h"
#include "shading/surface_element.h"

class Path_Tracer;

namespace scene
{
    /*  Scene/World class for holding the objects and lights in the scene. 
        The user is not expected to delete the pointers of added objects and lights, 
        those will be deleted in the destructor. */
    class Scene {
        friend class Path_Tracer;

    public:
        ~Scene();
        
        bool intersect(const Ray &ray, double& max_t, Surface_Element& surfel) const;

        void clear() { m_objects.clear(); m_point_lights.clear(); m_area_lights.clear(); };

        // Insertion functions
        void add_object(Object* ptr) { m_objects.push_back(ptr); }
        void add_point_light(Point_Light* ptr) { m_point_lights.push_back(ptr); }
        void add_area_light(Area_Light* ptr) { m_area_lights.push_back(ptr); }

    private:
        std::vector<Object*> m_objects;
        std::vector<Point_Light*> m_point_lights;
        std::vector<Area_Light*> m_area_lights;
    };
}

#endif

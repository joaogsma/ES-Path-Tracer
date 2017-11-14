#ifndef ES_PATH_TRACER__SCENE__SCENE_H_
#define ES_PATH_TRACER__SCENE__SCENE_H_

#include <vector>

#include "../geometry/vector3.h"
#include "../shading/color3.h"
#include "../shading/surface_element.h"
#include "area_light.h"
#include "object.h"

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
        
        bool intersect(const Ray &ray, double& max_t, Surface_Element& surfel,
			double refractive_index) const;

		void clear();

        // Insertion functions
        void add_object(Object* ptr) { m_objects.push_back(ptr); }
        //void add_point_light(Point_Light* ptr) { m_point_lights.push_back(ptr); }
		void add_area_light(Area_Light* ptr);

    private:
        std::vector<Object*> m_objects;
        //std::vector<Point_Light*> m_point_lights;
        std::vector<Area_Light*> m_area_lights;
		double m_total_light_area;
    };
}

#endif

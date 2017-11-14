#ifndef ES_PATH_TRACER__SCENE__MESH_OBJECT_H_
#define ES_PATH_TRACER__SCENE__MESH_OBJECT_H_

#include <vector>

#include "../geometry/aab.h"
#include "../geometry/ray.h"
#include "../geometry/triangle.h"
#include "../kd-tree/kd_tree.h"
#include "../shading/surface_element.h"
#include "object.h"

namespace scene
{
    class Mesh_Object : public Object {
    public:
		Mesh_Object(
			const std::vector<const Triangle*> &triangles, 
			Surface_Element::Material_Data material);
        
		bool intersect(const Ray &ray, double &t, Surface_Element& surfel) const;
        const AAB& aabb() const;

    private:
        kd_tree::KD_Tree m_kd_tree;
    };
}

#endif

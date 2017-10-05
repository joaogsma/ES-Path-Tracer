#ifndef __MESH_OBJECT_H__
#define __MESH_OBJECT_H__

#include <vector>

#include "geometry/aab.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "kd-tree/kd_tree.h"
#include "object.h"
#include "shading/surface_element.h"

namespace scene
{
    class Mesh_Object : public Object {
    public:
        Mesh_Object(const std::vector<const Triangle*> &triangles);
        
        bool intersect(const Ray &ray, double &t, Surface_Element& surfel) const;
        const AAB& aabb() const;

    private:
        kd_tree::KD_Tree m_kd_tree;
    };
}

#endif

#ifndef __MESH_OBJECT_H__
#define __MESH_OBJECT_H__

#include <vector>

#include "color/color3.h"
#include "color/bsdf.h"
#include "geometry/aab.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "kd-tree/kd_tree.h"
#include "object.h"

namespace scene
{
    class Mesh_Object : public Object {
    public:
        Mesh_Object(const std::vector<const Triangle*> &triangles, const BSDF &bsdf);
        
        bool hit(const Ray &ray, double &t, Color3 &color, Vector3 &normal) const;
        const AAB& aabb() const;

    private:
        kd_tree::KD_Tree mesh_kd_tree;
    };
}

#endif

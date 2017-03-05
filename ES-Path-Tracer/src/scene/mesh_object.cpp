#include <vector>

#include "color/color3.h"
#include "color/bsdf.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"
#include "scene/mesh_object.h"
#include "scene/object.h"

using std::vector;

namespace scene
{
    Mesh_Object::Mesh_Object(const vector<const Triangle*> &triangles, const BSDF &bsdf)
        : Object(bsdf), mesh_kd_tree(triangles) {}
    
    bool Mesh_Object::hit(const Ray &ray, double &t, Color3 &color, Vector3 &normal) const
    {
        const Triangle *hit_point = mesh_kd_tree.hit(ray);

        // TODO: compute color

        return false;
    }

    const AAB& Mesh_Object::aabb() const
    {
        return mesh_kd_tree.aabb();
    }
}
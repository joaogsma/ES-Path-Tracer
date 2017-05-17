#include <vector>

#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "scene/mesh_object.h"
#include "scene/object.h"
#include "scene/surface_element.h"

using std::vector;

namespace scene
{
    Mesh_Object::Mesh_Object(const vector<const Triangle*> &triangles) : m_kd_tree(triangles) {}
    
    bool Mesh_Object::intersect(const Ray &ray, double &t, Surface_Element& surfel) const
    {
        const Triangle *tri_ptr = m_kd_tree.intersect(ray);

        if ( !tri_ptr )
            return false;

        double t;
        std::vector<double> bar_weights;
        ray.intersect(*tri_ptr, t, bar_weights);

        // ========== Compute the shading normal ==========
        surfel.shading.normal = bar_weights[0] * (*tri_ptr->normal(0)) + 
            bar_weights[1] * (*tri_ptr->normal(1)) + 
            bar_weights[2] * (*tri_ptr->normal(2));
        // ================================================


        // ========== Compute the geometric normal ==========
        const Vector3& edge0 = Vector3(*tri_ptr->vertex(0), *tri_ptr->vertex(1));
        const Vector3& edge1 = Vector3(*tri_ptr->vertex(0), *tri_ptr->vertex(2));

        surfel.geometric.normal = cross_prod(edge0, edge1).normalize();
        // ==================================================
        

        // ========== Compute the geometric position ==========
        surfel.geometric.position = ray.origin + t * ray.direction;
        // ====================================================

        // TODO: tangent0 and tangent1

        return true;
    }

    const AAB& Mesh_Object::aabb() const
    {
        return m_kd_tree.aabb();
    }
}
#ifndef __GUARD_OBJECT_H__
#define __GUARD_OBJECT_H__

#include "geometry/ray.h"
#include "geometry/aab.h"
#include "scene/surface_element.h"

namespace scene
{
    class Object {
    public:
        virtual bool intersect(const Ray &ray, double &t, Surface_Element& surfel) const = 0;
        virtual const AAB& aabb() const = 0;

        virtual ~Object() {};
    };
}

#endif

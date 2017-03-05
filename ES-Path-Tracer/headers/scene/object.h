#ifndef __GUARD_OBJECT_H__
#define __GUARD_OBJECT_H__

#include "color/bsdf.h"
#include "geometry/ray.h"
#include "geometry/aab.h"

namespace scene
{
    class Object {
    public:
        BSDF bsdf;

        Object(const BSDF &bsdf) : bsdf(bsdf) {}

        virtual bool hit(const Ray &ray, double &t, Color3 &color, Vector3 &normal) const = 0;
        virtual const AAB& aabb() const = 0;

    protected:
        virtual ~Object() {};
    };
}

#endif

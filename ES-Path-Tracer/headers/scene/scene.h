#ifndef __GUARD_SCENE_H__
#define __GUARD_SCENE_H__

#include <vector>

#include "color/color3.h"
#include "geometry/vector3.h"
#include "object.h"

namespace scene
{
    class Scene {
    public:
        std::vector<Object> objects;

        bool hit(const Ray &ray, Color3 &color, Vector3 &normal);
    };
}

#endif

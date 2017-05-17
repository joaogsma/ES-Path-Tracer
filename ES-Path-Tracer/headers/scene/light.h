#ifndef __GUARD_LIGHT_H__
#define __GUARD_LIGHT_H__

#include "color/color3.h"

namespace scene
{
    class Light {
    public:
        Radiance3 power;

        Light(const Radiance3& power) : power(power) {}

        virtual ~Light() = 0;
    };

    Light::~Light() {}
}

#endif

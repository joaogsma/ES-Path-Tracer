#ifndef __GUARD_LIGHT_H__
#define __GUARD_LIGHT_H__

#include "shading/color3.h"

namespace scene
{
    class Light {
    public:
        Radiance3 m_power;

        Light(const Radiance3& m_power) : m_power(m_power) {}

        virtual ~Light() = 0;
    };

    inline Light::~Light() {}
}

#endif

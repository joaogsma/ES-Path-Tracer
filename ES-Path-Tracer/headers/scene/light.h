#ifndef ES_PATH_TRACER__SCENE__LIGHT_H_
#define ES_PATH_TRACER__SCENE__LIGHT_H_

#include "../shading/color3.h"

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

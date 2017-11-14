#ifndef ES_PATH_TRACER__SCENE__OBJECT_H_
#define ES_PATH_TRACER__SCENE__OBJECT_H_

#include "../geometry/ray.h"
#include "../geometry/aab.h"
#include "../shading/surface_element.h"

namespace scene
{
    class Object {
    public:
		Object(const Surface_Element::Material_Data& material) { set_material(material); }

		virtual bool intersect(const Ray &ray, double &t, Surface_Element& surfel) const = 0;
        virtual const AAB& aabb() const = 0;

		virtual const Surface_Element::Material_Data& material() const { return m_material; }
		virtual void set_material(const Surface_Element::Material_Data& material) { m_material = material; }
        
		virtual ~Object() {};

	protected:
		Surface_Element::Material_Data m_material;
    };
}

#endif

#include "geometry/plane.h"
#include "geometry/point3.h"
#include "geometry/vector3.h"

Plane::Plane(const Point3& p1, const Point3& p2, const Point3& p3)
    : point(p1), normal(cross_prod(Vector3(p1, p2), Vector3(p1, p3)).normalize()) {}

double Plane::evaluate(const Point3& p) const 
{
    return dot_prod(normal, Vector3(p, point));
}
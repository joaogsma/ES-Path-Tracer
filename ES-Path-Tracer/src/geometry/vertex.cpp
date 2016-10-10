#include "geometry/vector3.h"
#include "geometry/vertex.h"

Vertex::Vertex(double x, double y, double z, const Vector3 &normal) : 
	Surface_Point(x, y, z, normal), triangle_cout(0) {}
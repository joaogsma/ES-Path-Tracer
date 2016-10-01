#ifndef __GUARD_VECTOR3_H__
#define __GUARD_VECTOR3_H__

#include <vector>

class Vector3 {
public:
    double& x;
    double& y;
    double& z;

    Vector3(double x, double y, double z);

    Vector3 cross(Vector3 vector);

    Vector3 dot(double scalar);

    Vector3 normalize();

private:
    std::vector<double> coordinates;
};

#endif
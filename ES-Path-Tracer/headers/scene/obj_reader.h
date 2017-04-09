#ifndef __GUARD_OBJ_READER_H__
#define __GUARD_OBJ_READER_H__

#include <istream>
#include <fstream>
#include <vector>

#include "scene.h"

namespace scene
{
    bool read_mesh_object(std::istream& is, std::vector<Point3>& vertices, 
        std::vector<Vector3>& vertex_normals, std::vector<Triangle>& triangles);
}

#endif

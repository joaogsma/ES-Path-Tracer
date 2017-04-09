#include <istream>
#include <sstream>
#include <string>

#include "geometry/point3.h";
#include "geometry/vector3.h";
#include "scene/mesh_object.h"
#include "scene/obj_reader.h"
#include "scene/scene.h"

namespace scene
{
    bool read_vertex(std::istream& is, Point3& p)
    {
        double x, y, z, w = 1.0;

        if ( !(is >> x >> y >> z) )
            return false;

        if ( !is.eof() && !(is >> w) )
            return false;

        p.x = x / w;
        p.y = y / w;
        p.z = z / w;
        
        return true;
    }

    bool read_normal(std::istream& is, Vector3& n)
    {
        double x, y, z;

        if ( !(is >> x >> y >> z) )
            return false;

        n.x = x;
        n.y = y;
        n.z = z;

        return true;
    }

    bool read_mesh_object(std::istream& is, std::vector<Point3>& vertices,
        std::vector<Vector3>& vertex_normals, std::vector<Triangle>& triangles)
    {
        if (!is)
            return false;

        const std::string comment_command = "#";
        const std::string vertex_command = "v";
        const std::string face_command = "f";
        const std::string vertex_normal_command = "vn";

        std::string line;
        while ( std::getline(is, line) )
        {
            std::stringstream ss(line);

            std::string command;
            ss >> command;


            if (command == comment_command)
            {
                continue;
            }
            else if (command == vertex_command)
            {
                
            }
            else if (command == vertex_normal_command)
            {
                
            }
        }
    }

}
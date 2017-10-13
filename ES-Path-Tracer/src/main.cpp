#define _USE_MATH_DEFINES

#include "geometry/point3.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"
#include "scene/mesh_object.h"
#include "scene/scene.h"
#include "shading/surface_element.h"
#include "path-tracer/uniform_random_sequence.h"

#include "path-tracer/path_tracer.cpp"

#include <iostream>
#include <sstream>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::vector;

string point_to_string(Point3 p)
{
	stringstream ss;
	ss << "(" << p.x << ", " << p.y << ", " << p.z << ")";
	return ss.str();
}

string vector_to_string(Vector3 v)
{
	stringstream ss;
	ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return ss.str();
}

string color_to_string(Color3 c)
{
	stringstream ss;
	ss << "(" << c.r << ", " << c.g << ", " << c.b << ")";
	return ss.str();
}

int main()
{
	const float ground_y = -1.f;
	
	scene::Scene scene;

	vector<Point3> points = {
		Point3(0, 1, -2),
		Point3(-1.9, -1, -2),
		Point3(1.6, -0.5, -2),
		
		Point3(-10, ground_y, -10),
		Point3(-10, ground_y, -0.01f),
		Point3(10, ground_y, -0.01f),

		Point3(-10, ground_y, -10),
		Point3(10, ground_y, -0.01f),
		Point3(10, ground_y, -10),

		Point3(0, 2, 1),
		Point3(1, 3, 1),
		Point3(2, 2, 1)
	};

	vector<Vector3> normals = {
		Vector3(0, 0.6, 1).normalize(),
		Vector3(-0.4, -0.4, 1).normalize(),
		Vector3(0.4, -0.4, 1).normalize(),

		Vector3(0, 1, 0),
		Vector3(0, 1, 0),
		Vector3(0, 1, 0),

		Vector3(0, 1, 0),
		Vector3(0, 1, 0),
		Vector3(0, 1, 0),

		Vector3(0, 0, -1),
		Vector3(0, 0, -1),
		Vector3(0, 0, -1)
	};

	vector<Triangle> triangles;
	vector<scene::Object*> objects;
	
	for (int i = 0; i < 12; i += 3)
	{
		triangles.push_back(
			Triangle(
				&points[i],
				&points[i + 1],
				&points[i + 2],
				&normals[i],
				&normals[i + 1],
				&normals[i + 2]));
	}

	// ==================== Add the triangle ====================
	scene::Surface_Element::Material_Data triangle_material(
		Irradiance3(0),
		Color3(0.0, 0.8, 0.0),
		Color3(0.2, 0.2, 0.2),
		Color3(0.0),
		5,
		1.0,
		1.0);

	objects.push_back(
		new scene::Mesh_Object(std::vector<const Triangle*>(1, &triangles[0]), triangle_material));
	
	// ==========================================================


	// ==================== Add the ground ====================
	scene::Surface_Element::Material_Data ground_material(
		Irradiance3(0),
		Color3(0.8f, 0.8f, 0.8f),
		Color3(0.0, 0.0, 0.0),
		Color3(0.0),
		5,
		1.0,
		1.0);
	
	vector<const Triangle*> ground_triangle_pointers = { &triangles[1], &triangles[2] };
	objects.push_back(new scene::Mesh_Object(ground_triangle_pointers, ground_material));
	// ========================================================

	
	// Add the light source
	scene.add_area_light(new scene::Area_Light(Radiance3(10.0), std::vector<const Triangle*>(1, &triangles[3])));

	for (scene::Object* obj : objects)
		scene.add_object(obj);

	Path_Tracer path_tracer;

	Uniform_Random_Sequence rnd;

	Radiance3 result = path_tracer.path_trace(
		Ray(Point3(0), Vector3(Point3(0), Point3(-0.1, -1.0 / 6.0, -2.0))),
		scene,
		rnd,
		true);

	std::cout << color_to_string(result) << std::endl;

	return 0;
}
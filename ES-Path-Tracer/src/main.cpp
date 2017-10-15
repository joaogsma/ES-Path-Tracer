#define _USE_MATH_DEFINES

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "geometry/point3.h"
#include "geometry/ray.h"
#include "geometry/triangle.h"
#include "geometry/vector3.h"
#include "scene/mesh_object.h"
#include "scene/scene.h"
#include "shading/surface_element.h"
#include "path-tracer/camera.h"
#include "path-tracer/path_tracer.h"
#include "path-tracer/uniform_random_sequence.h"

int ppm_gamma_encode(float radiance, double d)
{
	return int(
		std::pow(
			std::min(1.0, std::max(0.0, radiance * d)),
			1.0 / 2.2) * 255.0);
}

void save_image(const std::string& filename, const std::vector<std::vector<Radiance3>>& image, double d)
{
	std::stringstream ss;

	int height = image.size();
	int width = image.front().size();

	ss << "P3 " << width << ' ' << height << ' ' << 255 << std::endl;

	for (int y = 0; y < height; ++y)
	{
		ss << std::endl << "# y = " << y << std::endl;
		for (int x = 0; x < width; ++x)
		{
			const Radiance3& color(image[y][x]);
			ss << ppm_gamma_encode(color.r, d)
				<< ' ' << ppm_gamma_encode(color.g, d)
				<< ' ' << ppm_gamma_encode(color.b, d)
				<< std::endl;
		}
	}

	std::ofstream ofs(filename);

	if (!(ofs << ss.str()))
		std::cout << "ERROR: Could not write to file" << std::endl;

	ofs.close();
}

int main()
{
	const float ground_y = -1.f;
	
	scene::Scene scene;

	std::vector<Point3> points = {
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

	std::vector<Vector3> normals = {
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

	std::vector<Triangle> triangles;
	std::vector<scene::Object*> objects;
	
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
		Color3(0.0, 0.0, 0.0),
		Color3(0.0, 0.0, 0.0),
		Color3(0.8, 0.8, 0.8),
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
	
	std::vector<const Triangle*> ground_triangle_pointers = { &triangles[1], &triangles[2] };
	objects.push_back(new scene::Mesh_Object(ground_triangle_pointers, ground_material));
	// ========================================================

	
	// Add the light source
	scene.add_area_light(new scene::Area_Light(Radiance3(10.0), std::vector<const Triangle*>(1, &triangles[3])));

	for (scene::Object* obj : objects)
		scene.add_object(obj);

	Uniform_Random_Sequence random_seq;

	Camera camera(Point3(0, 1, 6), Vector3(0, -0.1, -1), Vector3(0, 1, 0), 3);
	Path_Tracer path_tracer(
		&camera,
		&scene,
		3,           // Window width
		4./3.,      // Aspect ratio
		680,         // Width resolution
		100,         // Samples per pixel
		6);          // Number of threads
	std::vector<std::vector<Radiance3>> image;
	path_tracer.compute_image(image);

	std::string filename = "result_image.ppm";
	save_image(filename, image, 7);

	cv::Mat img_file = cv::imread(filename);

	if (!img_file.data)
	{
		std::cout << "Error: could not read image" << std::endl;
		return -1;
	}

	cv::imshow("Image", img_file);
	cv::waitKey(0);

	return 0;
}
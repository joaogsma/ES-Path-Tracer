#define _USE_MATH_DEFINES

#define ES_PATH_TRACER

#include <chrono>
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
#include "scene/sphere.h"
#include "shading/surface_element.h"
#include "path-tracer/camera.h"
#include "path-tracer/path_tracer.h"
#include "path-tracer/monte_carlo_path_tracer.h"
#include "path-tracer/evolution_strategy_path_tracer.h"
#include "random/uniform_random_sequence.h"

///////////////////////////////////////////////////////////////////////////////
//// General parameters
///////////////////////////////////////////////////////////////////////////////
const int N_THREADS = 6;
const int WIDTH_RESOLUTION = 600;
const double WINDOW_WIDTH = 3.0;
const double ASPECT_RATIO = 16.0 / 9.0;
const double GAMMA_ENCODING_COEFFICIENT = 6;
const double GAMMA_ENCODING_EXPONENT = 1.0 / 2.5;

///////////////////////////////////////////////////////////////////////////////
//// Evolution Strategy parameters
///////////////////////////////////////////////////////////////////////////////
const int MAX_ITERATIONS_PER_PIXEL = 10;
const int POPULATION_SIZE = 10;
const double CHILDREN_POPULATION_RATIO = 2.0;

///////////////////////////////////////////////////////////////////////////////
//// Vanilla Path Tracer parameters
///////////////////////////////////////////////////////////////////////////////
const int SAMPLES_PER_PIXEL = 200;

void save_image(const std::string& filename, const std::vector<std::vector<Radiance3>>& image)
{
	std::stringstream ss;

	int height = (int) image.size();
	int width = (int) image.front().size();

	ss << "P3 " << width << ' ' << height << ' ' << 255 << std::endl;

	for (int y = 0; y < height; ++y)
	{
		ss << std::endl << "# y = " << y << std::endl;
		for (int x = 0; x < width; ++x)
		{
			const Radiance3& color(image[y][x]);
			ss << (int) color.r << ' ' << (int) color.g << ' ' << (int) color.b << std::endl;
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
		Point3(2, 2, 1),

		Point3(3, -1, -4),
		Point3(3 + 1e-4, -1, 1),
		Point3(3, 2, 1),

		Point3(3, -1, -4),
		Point3(3, 2, 1),
		Point3(3 - 1e-4, 2, -4)
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
		Vector3(0, 0, -1),

		Vector3(-1, 0, 0).normalize(),
		Vector3(-1, 0, 0).normalize(),
		Vector3(-1, 0, 0).normalize(),

		Vector3(-1, 0, 0).normalize(),
		Vector3(-1, 0, 0).normalize(),
		Vector3(-1, 0, 0).normalize()
	};
	std::vector<Triangle> triangles;
	std::vector<scene::Object*> objects;
	
	for (int i = 0; i < points.size(); i += 3)
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
		Color3(0.0, 0.0, 0.0),
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


	// ==================== Add the wall ====================
	scene::Surface_Element::Material_Data wall_material(
		Irradiance3(0),
		Color3(0.6f, 0.0, 0.0),
		Color3(0.0),
		Color3(0.0),
		5,
		1.0,
		1.0);

	std::vector<const Triangle*> wall_triangle_pointers = { &triangles[4], &triangles[5] };
	objects.push_back(new scene::Mesh_Object(wall_triangle_pointers, wall_material));
	// ========================================================


	// ==================== Add the sphere ====================
	scene::Surface_Element::Material_Data sphere_material(
		Irradiance3(0),
		Color3(0),
		Color3(0.1),
		Color3(0.9),
		1e6,
		1.0,
		1.0);

	objects.push_back(new scene::Sphere(Point3(2, -0.4, -1.5), 0.5, sphere_material));
	// ========================================================
	

	// Add the light source
	scene.add_area_light(new scene::Area_Light(Radiance3(10.0), std::vector<const Triangle*>(1, &triangles[3])));

	for (scene::Object* obj : objects)
		scene.add_object(obj);

	random::Uniform_Random_Sequence random_seq;

	Camera camera(Point3(0, 1, 6), Vector3(0, -0.1, -1), Vector3(0, 1, 0), 3);
	
#ifdef ES_PATH_TRACER
	Path_Tracer& path_tracer = Evolution_Strategy_Path_Tracer(
		&camera,
		&scene,
		WINDOW_WIDTH,
		ASPECT_RATIO,
		WIDTH_RESOLUTION,
		MAX_ITERATIONS_PER_PIXEL,
		CHILDREN_POPULATION_RATIO,
		POPULATION_SIZE,
		GAMMA_ENCODING_COEFFICIENT,
		GAMMA_ENCODING_EXPONENT,
		N_THREADS);
#else
	Path_Tracer& path_tracer = Monte_Carlo_Path_Tracer(
		&camera,
		&scene,
		WINDOW_WIDTH,
		ASPECT_RATIO,
		WIDTH_RESOLUTION,
		SAMPLES_PER_PIXEL,
		GAMMA_ENCODING_COEFFICIENT,
		GAMMA_ENCODING_EXPONENT,
		N_THREADS);
#endif

	std::chrono::time_point<std::chrono::steady_clock> begin_instant = std::chrono::steady_clock::now();
	std::vector<std::vector<Radiance3>> image;
	path_tracer.compute_image(image);
	std::chrono::time_point<std::chrono::steady_clock> end_instant = std::chrono::steady_clock::now();

	long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_instant - begin_instant).count();
	std::cout << "Elapsed run time: " << elapsed << std::endl;

	std::string filename = "result_image.ppm";
	save_image(filename, image);

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
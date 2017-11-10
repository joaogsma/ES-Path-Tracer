#define _USE_MATH_DEFINES

#include <cmath>

#include "geometry/vector3.h"
#include "random/random_sequence.h"

namespace random
{
	double Random_Sequence::next_element()
	{
		return next();
	}

	Vector3 Random_Sequence::uniform_distributed_hemisphere_sample()
	{
		double u = next_element();
		double v = next_element();
		double r = sqrt(1 - v*v);

		return Vector3(r * cos(2 * M_PI * u), v, r * sin(2 * M_PI * u));
	}

	Vector3 Random_Sequence::cos_distributed_hemisphere_sample()
	{
		std::uniform_real_distribution<double> dist(0.0, 1.0);

		double theta = next_element() * 2 * M_PI;
		double s = next_element();

		double y = sqrt(s);
		double r = sqrt(1.0 - y*y);

		return Vector3(r * cos(theta), y, r * sin(theta));
	}
}
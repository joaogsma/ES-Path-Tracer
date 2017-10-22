#ifndef __GUARD_FITNESS_H__
#define __GUARD_FITNESS_H__

#include "../path-tracer/color_histogram.h"
#include "../path-tracer/path_tracer.h"
#include "individual.h"

namespace es
{
	class Color_Histogram_Fitness {
	public:
		Color_Histogram_Fitness(const Path_Tracer& path_tracer, int radius = 4);

		double operator()(const Individual& individual);

		// Acessor functions
		const Color_Histogram& color_histogram() const { return m_color_histogram; }
		int radius() const { return m_radius; }
		// Setters
		void set_color_histogram(const Color_Histogram& histogram);
		void set_radius(int radius);

	private:
		const Path_Tracer m_path_tracer;
		Color_Histogram m_color_histogram;
		int m_radius;
	};

}

#endif
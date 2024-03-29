#ifndef ES_PATH_TRACER__PATH_TRACER__COLOR_HISTOGRAM_H_
#define ES_PATH_TRACER__PATH_TRACER__COLOR_HISTOGRAM_H_

#include <unordered_map>

#include "../shading/color3.h"

class Evolution_Strategy_Path_Tracer;

class Color_Histogram {
	friend Evolution_Strategy_Path_Tracer;
public:
	Color_Histogram() : m_total_samples(0) {}

    // Increases counter for the specified color
    void increase(int red, int green, int blue);

    /*  Computation of the mean color based on the probabilities in the 
        histogram. This is equivalent to the summation of (p * color),
        for all values of color and its corresponding estimated 
        probabilities p */
    Color3 weighted_mean_color() const;

    /*  Computation of the information quantity associated with a given color.
        The information quantity is defined as -log(p), where p is the accumulated 
		probability of the given color and all others within the given radius */
    double information_quantity(int red, int green, int blue, int radius = 1) const;

private:
	std::unordered_map<int, unsigned int> m_color_histogram;
    long m_total_samples;

	int to_hash_key(int red, int green, int blue) const;

	Color3 from_hash_key(int key) const;
};

#include "evolution_strategy_path_tracer.h"

#endif
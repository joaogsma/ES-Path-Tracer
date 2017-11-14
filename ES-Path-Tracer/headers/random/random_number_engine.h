#ifndef ES_PATH_TRACER__RANDOM__RANDOM_NUMBER_ENGINE_H_
#define ES_PATH_TRACER__RANDOM__RANDOM_NUMBER_ENGINE_H_

#include <random>

namespace random
{
	std::mt19937& mt_engine_singleton();
}

#endif
#ifndef __GUARD_RANDOM_NUMBER_ENGINE_H__
#define __GUARD_RANDOM_NUMBER_ENGINE_H__

#include <random>

namespace random
{
	std::mt19937& mt_engine_singleton();
}

#endif
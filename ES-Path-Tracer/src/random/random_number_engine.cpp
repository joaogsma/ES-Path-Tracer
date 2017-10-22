#include <random>

#include "random/random_number_engine.h"

namespace random
{
	std::mt19937& mt_engine_singleton()
	{
		static std::mt19937 mt_engine = std::mt19937(std::random_device()());
		return mt_engine;
	}
}
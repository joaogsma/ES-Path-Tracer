#ifndef __GUARD_RANDOM_ENGINE_H__
#define __GUARD_RANDOM_ENGINE_H__

#include <random>

namespace es
{
	static std::random_device rd;
	static std::mt19937 mt_engine( rd() );
}

#endif

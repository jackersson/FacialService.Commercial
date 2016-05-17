#include "utils\utils.hpp"
#include <chrono>

namespace BioFacialEngine
{
	long Utils::getTicks()
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

		auto   value = now_ms.time_since_epoch();
		return (long)value.count();
	}	
}

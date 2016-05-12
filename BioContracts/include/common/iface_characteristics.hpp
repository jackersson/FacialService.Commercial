#ifndef IFaceCharacteristics_INCLUDED
#define IFaceCharacteristics_INCLUDED

#include "service_context.hpp"

namespace BioContracts
{
	class IFaceCharacteristics
	{
	public:
		virtual unsigned int age() const = 0;
	};
}

#endif
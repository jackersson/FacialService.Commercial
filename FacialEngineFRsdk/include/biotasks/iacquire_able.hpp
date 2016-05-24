#ifndef IAcquireAble_INCLUDED
#define IAcquireAble_INCLUDED

#include "ienrollment_processor.hpp"

#include <string>
namespace BioFacialEngine
{
	class IAcquireAble
	{
	public:
		virtual void acquire(IAcquisitionProcessor& processor) = 0;
	};
}

#endif
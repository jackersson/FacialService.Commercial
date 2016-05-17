#ifndef IEnrollmentAble_INCLUDED
#define IEnrollmentAble_INCLUDED

#include "ienrollment_processor.hpp"

#include <string>
namespace BioFacialEngine
{
	class IEnrollmentAble
	{
	public:
		virtual void enroll(IEnrollmentProcessor& processor) = 0;
	};
}

#endif
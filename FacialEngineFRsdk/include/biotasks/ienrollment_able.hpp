#ifndef IEnrollmentAble_INCLUDED
#define IEnrollmentAble_INCLUDED

#include <string>

//#include <frsdk\enroll.h>
#include "ienrollment_processor.hpp"
namespace BioFacialEngine
{
	class IEnrollmentAble
	{
	public:
		virtual void enroll(IEnrollmentProcessor& processor) = 0;
	};
}

#endif
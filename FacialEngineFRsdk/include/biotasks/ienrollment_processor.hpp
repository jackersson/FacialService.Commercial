#ifndef IEnrollmentProcessor_INCLUDED
#define IEnrollmentProcessor_INCLUDED

#include <string>

#include <frsdk\image.h>
namespace BioFacialEngine
{
	class IEnrollmentProcessor
	{
	public:
		virtual std::string enroll(const FRsdk::Image& image) = 0;
	};
}

#endif
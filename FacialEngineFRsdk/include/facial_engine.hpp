#ifndef FacialEngine_INCLUDED
#define FacialEngine_INCLUDED

#include "engine/ifacial_engine.hpp"

#include "biotasks/facial_acquisition.hpp"
#include "biotasks/facial_enrollment.hpp"
#include "biotasks/facial_verification.hpp"

namespace BioFacialEngine
{

	class FacialEngine
	{
	public:
		FacialEngine(const std::string& frsdk_configuration);
		~FacialEngine() {}


	private:
		FacialEngine(const FacialEngine&);

	private:
		std::unique_ptr<FacialAcquisition>  acquisition_ ;
		std::unique_ptr<FacialEnrollmentProcessor>   enrollment_  ;
		std::unique_ptr<FacialVerification> verification_;



	};
}

#endif
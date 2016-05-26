
#include "facial_engine.hpp"
#include "utils/face_vacs_io_utils.hpp"

#include <Windows.h>
#include <ppl.h>

namespace BioFacialEngine
{

	FacialEngine::FacialEngine( const std::string& frsdk_configuration)
		                        : acquisition_ ( new FacialAcquisition (frsdk_configuration))
		                        , enrollment_  ( new FacialEnrollmentProcessor  (frsdk_configuration))
														, verification_( new FacialVerification(frsdk_configuration))
	{
	}

	


	
}

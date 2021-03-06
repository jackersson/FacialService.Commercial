#include "facial_engine.hpp"

using namespace Pipeline;

sitmo::prng_engine FRsdkEntities::ImageInfo::randomizer_;
namespace BioFacialEngine
{
	
	FacialEngine::FacialEngine( const std::string& frsdk_configuration)		                       
	{
		processor_ = std::make_shared<FRsdkBiometricProcessor>(frsdk_configuration);
	  pipeline_ = std::make_unique <BiometricPipelineBalanced>(processor_);
	}


	
	

	
}

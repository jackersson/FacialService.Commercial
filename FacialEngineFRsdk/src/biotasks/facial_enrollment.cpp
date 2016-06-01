#include "biotasks/facial_enrollment.hpp"

using namespace FRsdkTypes;

namespace BioFacialEngine
{
	FacialEnrollmentProcessor::FacialEnrollmentProcessor(const std::string& configuration_filename)
	{
	
		try
		{
			FaceVacsConfiguration configuration(std::make_shared<FRsdk::Configuration>(configuration_filename));
			init(configuration);
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}
	}


	FacialEnrollmentProcessor::FacialEnrollmentProcessor(FaceVacsConfiguration configuration)
	{
		init(configuration);
	}

	bool FacialEnrollmentProcessor::init(FaceVacsConfiguration configuration)
	{
		try
		{						
			load_balancer_ = std::make_shared<EnrollmentLoadBalancer>(configuration);
			return true;
		}	
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}	
}
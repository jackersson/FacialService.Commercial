#include "biotasks/enrollment/facial_enrollment.hpp"

using namespace FRsdkTypes;

namespace BioFacialEngine
{
	FacialEnrollment::FacialEnrollment(const std::string& configuration_filename)
	{	
		try
		{
			auto configuration(std::make_shared<FRsdk::Configuration>(configuration_filename));
			init(configuration);
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}
	}

	FacialEnrollment::FacialEnrollment(FaceVacsConfiguration configuration)	{
		init(configuration);
	}

	bool FacialEnrollment::init(FaceVacsConfiguration configuration)
	{
		try
		{						
			load_balancer_ = std::make_shared<EnrollmentLoadBalancer>(configuration);
			return true;
		}	
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}	

	void FacialEnrollment::enroll(FaceVacsImage image, FRsdk::Enrollment::Feedback& feedback) const	{
		load_balancer_->enroll(image, feedback);	
	}
}
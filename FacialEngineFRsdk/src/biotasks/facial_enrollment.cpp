#include "biotasks/facial_enrollment.hpp"
#include "feedback/facial_enrollment_feedback.hpp"



namespace BioFacialEngine
{
	FacialEnrollment::FacialEnrollment(const std::string& configuration_filename)
	{
		bool is_ok = false;
		try
		{
			FaceVacsConfiguration configuration = new FRsdk::Configuration(configuration_filename);
			is_ok = init(configuration);
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}

		if (!is_ok)
			this->~FacialEnrollment();
	}


	FacialEnrollment::FacialEnrollment(FaceVacsConfiguration configuration)
	{
		if (!init(configuration))
			this->~FacialEnrollment();
	}

	bool FacialEnrollment::init(FaceVacsConfiguration configuration)
	{
		try
		{			
			EnrollmentProcessorPoolPtr pool(new EnrollmentProcessorPool(configuration));
			pool_ = pool;			
			return true;
		}	
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}
	
	FRsdkFir FacialEnrollment::build(const std::string& bytes)
	{
		return fir_builder_->build(bytes);
		//return result;
	}
}
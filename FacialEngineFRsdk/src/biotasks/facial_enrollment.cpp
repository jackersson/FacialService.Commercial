#include "biotasks/facial_enrollment.hpp"


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
			processor_ = new FRsdk::Enrollment::Processor(*configuration);
			return true;
		}	
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}

	std::string FacialEnrollment::enroll(const FRsdk::Image& image)
	{		
		std::shared_ptr<std::string> fir_bytestring(new std::string(""));

		FRsdk::SampleSet images;
		images.push_back(image);

		FRsdk::Enrollment::Feedback
			feedback(new FacialEnrollmentFeedback(*(fir_bytestring.get())));

		processor_->process(images.begin(), images.end(), feedback);		

		return *fir_bytestring;
	}

	void FacialEnrollment::enroll(IEnrollmentAble& face_characteristic)
	{
		face_characteristic.enroll(*this);	
	}

	void FacialEnrollment::enroll(const std::string& filename)
	{

	}
}
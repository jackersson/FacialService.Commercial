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
			processor_   = new FRsdk::Enrollment::Processor(*configuration);

			
			//std::shared_ptr<FirBilder> ptr(new FirBilder(configuration));
			//fir_builder_ = ptr;
			return true;
		}	
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}

	
	BioContracts::IdentificationRecordRef FacialEnrollment::enroll(const FRsdk::Image& image)
	{		
		BioContracts::IdentificationRecordRef fir(new BioContracts::IdentificationRecord(""));
		enroll(image, fir);
		return fir;
	}

	void FacialEnrollment::enroll(const FRsdk::Image& image, BioContracts::IdentificationRecordRef fir)
	{		
		FRsdk::SampleSet images;
		images.push_back(image);

		//FRsdk::Enrollment::Feedback
			//feedback(new FacialEnrollmentFeedback(fir));

		//processor_->process(images.begin(), images.end(), feedback);
	}

	void FacialEnrollment::enroll(IEnrollmentAble& enrollment_able)
	{
		enrollment_able.enroll(*this);
	}
	
	/*
	void FacialEnrollment::enroll(FRsdkFaceCharacteristic& face_characteristic)
	{
		face_characteristic.enroll(*this);	
	}
	*/

	void FacialEnrollment::enroll(const std::string& filename)
	{

	}

	FRsdkFir FacialEnrollment::build(const std::string& bytes)
	{
		return fir_builder_->build(bytes);
		//return result;
	}
}
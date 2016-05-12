#include "biotasks/facial_verification.hpp"


namespace BioFacialEngine
{
	FacialVerification::FacialVerification(const std::string& configuration_filename)
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
			this->~FacialVerification();
	}


	FacialVerification::FacialVerification(FaceVacsConfiguration configuration)
	{
		if (!init(configuration))
			this->~FacialVerification();
	}

	bool FacialVerification::init(FaceVacsConfiguration configuration)
	{
		try
		{
			processor_      = new FRsdk::Verification::Processor(*configuration);
			score_mappings_ = new FRsdk::ScoreMappings(*configuration);
			return true;
		}
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}
	/*
	void FacialVerification::verify(FaceVacsImage image, FacialEnrollmentFeedback feedback)
	{
	
	}
	*/

	float FacialVerification::verify(FRsdkFaceCharacteristic& face_characteristic, const FRsdk::FIR& fir)
	{
		FRsdk::SampleSet images;
		images.push_back(face_characteristic.annotatedImage());

		FRsdk::CountedPtr<FRsdk::Score> result = new FRsdk::Score(.0f);

		FRsdk::Verification::Feedback
			feedback(new FacialVerificationFeedback(*result));

		processor_->process(  images.begin(), images.end()
			                  , fir
			                  , score_mappings_->requestFAR(FAR_THRESHOLD)
			                  , feedback);

		return *result;
	}

	/*
	void FacialVerification::verify(const std::string& filename)
	{

	}
	*/
}
#include "biotasks/facial_verification.hpp"

#include "feedback\facial_verification_feedback.hpp"

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

			std::shared_ptr<FirBilder> ptr(new FirBilder(configuration));
			fir_builder_ = ptr;
			return true;
		}
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}

	/*
	float FacialVerification::verify(FRsdkFaceCharacteristic& face_characteristic, const FRsdk::FIR& fir)
	{

		if (fir.size() <= 0)
			throw new std::exception("compareTo fir is NULL");

		
		return 0.0f /* verify(face_characteristic.annotatedImage(), fir);
	}


	
	float FacialVerification::verify(FRsdkFaceCharacteristic& target_fc, FRsdkFaceCharacteristic& verifier_fc)
	{	
		if (!verifier_fc.hasFir())
			return 0.0f;

		FRsdk::Image image = target_fc.annotatedImage();
		FRsdk::FIR   fir   = verifier_fc.fir();

		return 0.0/* verify(image, fir);
	}
	*/

	float FacialVerification::verify(const FRsdk::Image& image, const FRsdk::FIR& fir)
	{	
		FRsdk::SampleSet images;
		images.push_back(image);

		FRsdk::CountedPtr<FRsdk::Score> result = new FRsdk::Score(.0f);

		FRsdk::Verification::Feedback
			feedback(new FacialVerificationFeedback(*result));

		std::cout << fir.size() << std::endl;

		processor_->process( images.begin(), images.end()
			                 , fir
			                 , 0.01f
			                 , feedback);

		return *result;
	}
	/*
	BioContracts::Matches FacialVerification::verify( ImageCharacteristicsType& target
		                                              , ImageCharacteristicsType& comparison)
	{
		BioContracts::Matches matches;
		
		for (int i = 0; i != target->size(); ++i)
		{
			for (int j = 0; j != comparison->size(); ++j)
			{
				FRsdkFaceCharacteristic first = (*target)[i];
				FRsdkFaceCharacteristic second = (*target)[j];

				float match = first.compareTo(second, *this);

			

				BioContracts::Match mat(first.id(), second.id(), match);
				matches.Add(mat);
			}
		}

		return matches;
	}
	*/

	FRsdkFir FacialVerification::build(const std::string& bytes)
	{
		return fir_builder_->build(bytes);
		//return result;
	}
	
}
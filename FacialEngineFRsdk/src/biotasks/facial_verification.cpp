#include "biotasks/facial_verification.hpp"


using namespace FRsdkTypes;

namespace BioFacialEngine
{
	FacialVerification::FacialVerification(const std::string& configuration_filename)
	{		
		try
		{
			FaceVacsConfiguration configuration( new FRsdk::Configuration(configuration_filename) );
			init(configuration);
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}		
	}


	FacialVerification::FacialVerification(FaceVacsConfiguration configuration)
	{
		init(configuration);
	}

	bool FacialVerification::init(FaceVacsConfiguration configuration)
	{
		try
		{
			processor_      = new FRsdk::Verification::Processor(*configuration);
			score_mappings_ = new FRsdk::ScoreMappings(*configuration);

			fir_builder_ = std::make_shared<FirBilder>(configuration);
			return true;
		}
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}
	
	float FacialVerification::verify(const FRsdk::Image& image, const FRsdk::FIR& fir)
	{	
		FRsdk::SampleSet images;
		images.push_back(image);

		//FRsdk::CountedPtr<FRsdk::Score> result = new FRsdk::Score(.0f);

		//FRsdk::Verification::Feedback
			//feedback(new FacialVerificationFeedback(*result));

		//std::cout << fir.size() << std::endl;

		//processor_->process( images.begin(), images.end()
			               //  , fir
			               //  , 0.01f
			            //     , feedback);

		return 0.1f;
	}


	FRsdkFir FacialVerification::build(const std::string& bytes) const
	{
		return fir_builder_->build(bytes);		
	}
	
}
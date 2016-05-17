#ifndef FacialVerification_INCLUDED
#define FacialVerification_INCLUDED

#include "biotasks\ifir_builder.hpp"
#include "biotasks\iverification_processor.hpp"
#include "feedback\facial_verification_feedback.hpp"
#include "common\matches.hpp"

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Verification::Processor> FRsdkVerificationProcessor;
	typedef FRsdk::CountedPtr<FRsdk::ScoreMappings>           FRsdkScoreMappings        ;

	class FacialVerification : public IVerificationProcessor
	{
	public:
		FacialVerification(const std::string& configuration_filename);
		FacialVerification(FaceVacsConfiguration configuration);
		~FacialVerification(){}
						
		//void verify(const std::string& imagefilename, const std::string& firfilename  );

		//float verify(FRsdkFaceCharacteristic& target_fc, FRsdkFaceCharacteristic& verifier_fc );
		//float verify(FRsdkFaceCharacteristic& target_fc, const FRsdk::FIR& fir                );
		float verify(const FRsdk::Image& image, const FRsdk::FIR& fir);

		//BioContracts::Matches verify(ImageCharacteristicsType& target, ImageCharacteristicsType& comparison);

		FRsdkFir build(const std::string& bytes);

	private:
		bool init(FaceVacsConfiguration configuration);

	private:
		FRsdkVerificationProcessor processor_     ;
		FRsdkScoreMappings         score_mappings_;
		FirBuilderRef              fir_builder_   ;
	private:
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;

		const float FAR_THRESHOLD    = 0.001f;
	};
}

#endif
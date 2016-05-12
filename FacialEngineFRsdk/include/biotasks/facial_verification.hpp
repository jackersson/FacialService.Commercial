#ifndef FacialVerification_INCLUDED
#define FacialVerification_INCLUDED

#include "engine/ifacial_engine.hpp"
#include "biotasks/facial_acquisition.hpp"
#include "feedback/facial_enrollment_feedback.hpp"

#include <frsdk/verify.h>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Verification::Processor> FRsdkVerificationProcessor;
	typedef FRsdk::CountedPtr<FRsdk::ScoreMappings>           FRsdkScoreMappings        ;

	class FacialVerification
	{

	public:
		FacialVerification(const std::string& configuration_filename);
		FacialVerification(FaceVacsConfiguration configuration);
		~FacialVerification(){}
						
		//void verify(const std::string& imagefilename, const std::string& firfilename  );

		//float verify(FRsdkFaceCharacteristic& target_fc, FRsdkFaceCharacteristic& verifier_fc );
		float verify(FRsdkFaceCharacteristic& target_fc, const FRsdk::FIR& fir                );
		//float verify(FaceVacsImage image               , const FRsdk::FIR& fir                );

	private:
		bool init(FaceVacsConfiguration configuration);

	private:
		FRsdkVerificationProcessor processor_     ;
		FRsdkScoreMappings         score_mappings_;

	private:
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;

		const float FAR_THRESHOLD    = 0.001f;
	};
}

#endif
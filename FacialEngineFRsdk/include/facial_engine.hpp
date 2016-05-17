#ifndef FacialEngine_INCLUDED
#define FacialEngine_INCLUDED

#include "engine/ifacial_engine.hpp"

#include "biotasks/facial_acquisition.hpp"
#include "biotasks/facial_enrollment.hpp"
#include "biotasks/facial_verification.hpp"

namespace BioFacialEngine
{

	class FacialEngine : public BioContracts::IFacialEngine
	{
	public:
		FacialEngine(const std::string& frsdk_configuration);
		~FacialEngine() {}

		BioContracts::ImageCharacteristicsConstRef
		acquire(const std::string& filename);

		BioContracts::ImageCharacteristicsConstRef
		acquire(const BioContracts::RawImage& image);

		BioContracts::ImageCharacteristicsConstRef
	  enroll(const std::string& filename );

		ImageCharacteristicsType
		enrollFromFile(const std::string& filename);

		BioContracts::ImageCharacteristicsConstRef
		enroll(const BioContracts::RawImage& image);


		BioContracts::VerificationResult
			FacialEngine::verify(const std::string& first, const std::string& second);

		BioContracts::VerificationResult
		verify(const BioContracts::RawImage& image, const BioContracts::RawImage& comparison_raw_image);

	//	void identify(const BioService::VerificationData& images);

	private:
		FacialEngine(const FacialEngine&);

		ImageCharacteristicsType enrollPerformer(FaceVacsImage image );
		ImageCharacteristicsType enrollPerformer(const BioContracts::RawImage& raw_image);
	
	private:
		std::unique_ptr<FacialAcquisition>  acquisition_ ;
		std::unique_ptr<FacialEnrollment>   enrollment_  ;
		std::unique_ptr<FacialVerification> verification_;



	};
}

#endif
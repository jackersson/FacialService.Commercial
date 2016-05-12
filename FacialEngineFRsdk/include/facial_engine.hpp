#ifndef FacialEngine_INCLUDED
#define FacialEngine_INCLUDED

#include "engine/ifacial_engine.hpp"

#include "biotasks/facial_acquisition.hpp"
#include "biotasks/facial_enrollment.hpp"
#include "biotasks/facial_verification.hpp"

//#include "common/iimage_characteristic.hpp"

#include <memory>

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
		acquire(const std::string& image_bytestring, size_t size);

		BioContracts::ImageCharacteristicsConstRef
	  enroll(const std::string& filename );

		BioContracts::ImageCharacteristicsConstRef
		enroll(const std::string& image_bytestring, size_t size);



//		void verify  (const BioService::VerificationData& verification_data);

	//	void identify(const BioService::VerificationData& images);

	private:
		FacialEngine(const FacialEngine&);

	
	private:
		std::unique_ptr<FacialAcquisition>  acquisition_ ;
		std::unique_ptr<FacialEnrollment>   enrollment_  ;
		std::unique_ptr<FacialVerification> verification_;



	};
}

#endif
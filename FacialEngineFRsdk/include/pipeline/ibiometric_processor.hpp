#ifndef IBiometricProcessor_INCLUDED
#define IBiometricProcessor_INCLUDED

#include <wrappers/image_info.hpp>
#include <biotasks/verification/facial_verification.hpp>

#include <common/identification_result.hpp>
#include <biotasks/identification/facial_identification.hpp>
#include <common/verification_result.hpp>

namespace Pipeline
{
	class IBiometricProcessor
	{
	public:
		virtual ~IBiometricProcessor() {}

		virtual FRsdkEntities::ImageInfoPtr load_image(const std::string& filename) = 0;

		virtual void         face_find             (FRsdkEntities::ImageInfoPtr        pInfo) = 0;
		virtual void         portrait_analyze      (FRsdkEntities::FaceInfoPtr         pInfo) = 0;
		virtual void         extract_facial_image  (FRsdkEntities::FaceInfoPtr         pInfo) = 0;
		virtual void         iso_compliance_test   (FRsdkEntities::FaceInfoPtr         pInfo) = 0;
		virtual void         enroll                (FRsdkEntities::FaceInfoPtr         pInfo) = 0;

		virtual BioContracts::VerificationResultPtr
			      verify                             (BioFacialEngine::VerificationPair    pInfo) = 0;

		virtual BioContracts::IdentificationResultPtr
			      identify                           (BioFacialEngine::IdentificationPair  pInfo) = 0;
	};

	typedef std::shared_ptr<IBiometricProcessor> IBiometricProcessorPtr;
}
#endif

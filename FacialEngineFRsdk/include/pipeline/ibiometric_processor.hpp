#ifndef IBiometricProcessor_INCLUDED
#define IBiometricProcessor_INCLUDED

#include <wrappers/image_info.hpp>

namespace Pipeline
{
	class IBiometricProcessor
	{
	public:
		virtual ~IBiometricProcessor() {}

		virtual FRsdkEntities::ImageInfoPtr load_image(const std::string& filename) = 0;

		virtual void         face_find             (FRsdkEntities::ImageInfoPtr pInfo) = 0;
		virtual void         portrait_analyze      (FRsdkEntities::FaceInfoPtr  pInfo) = 0;
		virtual void         extract_facial_image(FRsdkEntities::FaceInfoPtr  pInfo) = 0;
		virtual void         iso_compliance_test   (FRsdkEntities::FaceInfoPtr  pInfo) = 0;
		virtual void         enroll                (FRsdkEntities::FaceInfoPtr  pInfo) = 0;
		virtual void         verify                (FRsdkEntities::FaceInfoPtr  pInfo) = 0;
	};

	typedef std::shared_ptr<IBiometricProcessor> IBiometricProcessorPtr;
}
#endif

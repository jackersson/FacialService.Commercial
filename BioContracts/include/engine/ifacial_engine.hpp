#ifndef IFacialEngine_INCLUDED
#define IFacialEngine_INCLUDED

#include "common/iimage_characteristic.hpp"
#include "common/raw_image.hpp"

#include "common/verification_result.hpp"
#include "common/identification_result.hpp"

namespace BioContracts
{
	class VerificationResult;

	class IFacialEngine
	{
	public:
		virtual ~IFacialEngine() {}

		virtual IImageInfoPtr
		acquire(const std::string& filename) = 0;

		virtual IImageInfoPtr
		acquire(const RawImage& raw_image) = 0;

		virtual VerificationResultPtr
		verify(const std::string& object, const std::string& subject) = 0;

		virtual VerificationResultPtr
		verify(const RawImage& object, const RawImage& subject) = 0;

		virtual IdentificationResultPtr
		identify(const std::string& object, const std::list<std::string>& subjects) = 0;

		virtual IdentificationResultPtr
		identify(const RawImage& object, const std::list<RawImage>& subjects) = 0;

		//virtual IImageInfoPtr
		//	enroll(const std::string& filename) = 0;

		//virtual IImageInfoPtr
			//enroll(const RawImage& raw_image) = 0;
	};

		
}

#endif
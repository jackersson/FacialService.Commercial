#ifndef IFacialEngine_INCLUDED
#define IFacialEngine_INCLUDED

#include "protobufs/bio_service.pb.h"
#include "common/iimage_characteristic.hpp"
#include "common/raw_image.hpp"
#include "common/verification_result.hpp"

namespace BioContracts
{
	class IFacialEngine
	{
	public:

		virtual ImageCharacteristicsConstRef
			acquire(const std::string& filename) = 0;

		virtual ImageCharacteristicsConstRef
			acquire(const RawImage& raw_image) = 0;

		virtual ImageCharacteristicsConstRef
			enroll(const std::string& filename) = 0;

		virtual ImageCharacteristicsConstRef
			enroll(const RawImage& raw_image) = 0;


		virtual BioContracts::VerificationResult verify( const RawImage& target_raw_image
			                                             , const RawImage& comparison_raw_image) = 0;

	//	virtual void identify(const BioService::AcquiredData& images) = 0;
	};

		
}

#endif
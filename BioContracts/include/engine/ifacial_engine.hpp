#ifndef IFacialEngine_INCLUDED
#define IFacialEngine_INCLUDED

#include "bio_service.pb.h"
#include "iimage_characteristic.hpp"
#include <string>
namespace BioContracts
{
	class IFacialEngine
	{
	public:

		virtual ImageCharacteristicsConstRef
			acquire(const std::string& filename) = 0;

		virtual ImageCharacteristicsConstRef
			acquire(const std::string& image_bytestring, size_t size) = 0;

		virtual ImageCharacteristicsConstRef
			enroll(const std::string& filename) = 0;

		virtual ImageCharacteristicsConstRef
			enroll(const std::string& image_bytestring, size_t size) = 0;


//		virtual void verify  (const BioService::AcquiredData& verification_data) = 0;

	//	virtual void identify(const BioService::AcquiredData& images) = 0;
	};

		
}

#endif
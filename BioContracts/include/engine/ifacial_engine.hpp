#ifndef IFacialEngine_INCLUDED
#define IFacialEngine_INCLUDED

#include "common/iimage_characteristic.hpp"
#include "common/raw_image.hpp"

namespace BioContracts
{
	class IFacialEngine
	{
	public:
		virtual ~IFacialEngine() {}

	//	virtual IImageInfoPtr
		//	acquire(const std::string& filename) = 0;

		virtual IImageInfoPtr
			acquire(const RawImage& raw_image) = 0;

		//virtual IImageInfoPtr
		//	enroll(const std::string& filename) = 0;

		//virtual IImageInfoPtr
			//enroll(const RawImage& raw_image) = 0;
	};

		
}

#endif
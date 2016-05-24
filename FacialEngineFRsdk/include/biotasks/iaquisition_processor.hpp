#ifndef IAcquisitionProcessor_INCLUDED
#define IAcquisitionProcessor_INCLUDED

#include <frsdk/image.h>

#include <memory>

namespace BioFacialEngine
{
	class IAcquisitionProcessor 
	{
	public:
		virtual BioContracts::IdentificationRecordRef enroll(const FRsdk::Image& image) = 0;

		virtual void                                  enroll(const FRsdk::Image& image
			, BioContracts::IdentificationRecordRef fir) = 0;
	};
}

#endif
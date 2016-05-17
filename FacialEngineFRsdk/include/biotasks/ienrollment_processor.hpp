#ifndef IEnrollmentProcessor_INCLUDED
#define IEnrollmentProcessor_INCLUDED

#include "biotasks/ifir_builder.hpp"
#include "common/identification_record.hpp"

#include <frsdk/image.h>

#include <memory>

namespace BioFacialEngine
{
	class IEnrollmentProcessor : public IFirBuilder
	{
	public:
		virtual BioContracts::IdentificationRecordRef enroll( const FRsdk::Image& image) = 0;

		virtual void                                  enroll( const FRsdk::Image& image
			                                                  , BioContracts::IdentificationRecordRef fir) = 0;
	};
}

#endif
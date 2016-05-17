#ifndef IVerificationProcessor_INCLUDED
#define IVerificationProcessor_INCLUDED

#include "biotasks\ifir_builder.hpp"

namespace BioFacialEngine
{
	class IVerificationProcessor : public IFirBuilder
	{
	public:
		virtual float verify(const FRsdk::Image& image, const FRsdk::FIR& fir) = 0;
	};
}

#endif
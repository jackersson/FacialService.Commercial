#ifndef IVerificationAble_INCLUDED
#define IVerificationAble_INCLUDED

#include "biotasks\iverification_processor.hpp"
namespace BioFacialEngine
{
	class IVerificationAble
	{
	public:
		virtual float verifyWith( const FRsdk::Image& face
			                      , IVerificationProcessor& processor) = 0;
	};
}

#endif
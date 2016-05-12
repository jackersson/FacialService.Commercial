#ifndef FacialEnrollmentFeedback_INCLUDED
#define FacialEnrollmentFeedback_INCLUDED

#include "engine/ifacial_engine.hpp"

#include <frsdk/config.h>
#include <frsdk/image.h>
#include <frsdk/tokenface.h>
#include <frsdk/portrait.h>
#include <frsdk/eyes.h>
#include <frsdk/cbeff.h>
#include <frsdk/fir.h>
#include <frsdk/enroll.h>

#include <functional>

namespace BioFacialEngine
{
	typedef std::function<void(const FRsdk::FIR& fir)> FirCallback;

	class FacialEnrollmentFeedback : public FRsdk::Enrollment::FeedbackBody
	{
	public:
		FacialEnrollmentFeedback(std::string& fir_bytestring) : fir_bytestring_(fir_bytestring) //setFir(setFirFunction), fir_(fir)
		{
			
		}
		~FacialEnrollmentFeedback() {}

		void start() {}
		void end() {}

		void processingImage(const FRsdk::Image& frame){}

		void eyesFound(const FRsdk::Eyes::Location& eyes){}
		void eyesNotFound() {}


		void success(const FRsdk::FIR& fir)
		{			
			std::stringstream fir_bytestring_stream;
			fir_bytestring_stream << fir;
			fir_bytestring_ = fir_bytestring_stream.str();				
		}

		void failure(){		
		}

		
	private:
		FacialEnrollmentFeedback(const FacialEnrollmentFeedback&);
		void operator=(const FacialEnrollmentFeedback&);

	private:
		std::string& fir_bytestring_;
		//FRsdk::FIR& fir_;
		FirCallback setFir;
	};
}
#endif
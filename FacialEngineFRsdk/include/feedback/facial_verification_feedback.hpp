#ifndef FacialVerificationFeedback_INCLUDED
#define FacialVerificationFeedback_INCLUDED

#include "engine/ifacial_engine.hpp"

#include <frsdk/config.h>
#include <frsdk/image.h>
#include <frsdk/tokenface.h>
#include <frsdk/portrait.h>
#include <frsdk/eyes.h>
#include <frsdk/cbeff.h>
#include <frsdk/fir.h>
#include <frsdk/enroll.h>
#include <frsdk/verify.h>

#include <functional>

namespace BioFacialEngine
{
	//typedef std::function<void(const FRsdk::FIR& fir)> FirCallback;

	class FacialVerificationFeedback : public FRsdk::Verification::FeedbackBody
	{
	public:
		FacialVerificationFeedback(FRsdk::Score& score/*const FirCallback& setFirFunction*/) 
			                         : score_(score) //:  setFir(setFirFunction)
		{}
		~FacialVerificationFeedback() {}

		void start() {}
		void end() {}

		void processingImage(const FRsdk::Image& frame){}

		void eyesFound(const FRsdk::Eyes::Location& eyes){}
		void eyesNotFound() {}

		void match(const FRsdk::Score& s)
		{
			score_ = s;
		}

		void success()
		{
			//setFir(fir);
		}

		void failure(){
		}


	private:
		FacialVerificationFeedback(const FacialVerificationFeedback&);
		void operator=(const FacialVerificationFeedback&);

	private:
		FRsdk::Score& score_;
		//FirCallback setFir;
	};
}
#endif
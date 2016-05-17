#ifndef FacialVerificationFeedback_INCLUDED
#define FacialVerificationFeedback_INCLUDED

#include <frsdk/verify.h>

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
			std::cout << s << std::endl;
		}

		void success()
		{
			//setFir(fir);
		}

		void failure(){
			std::cout << "t" << std::endl;
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
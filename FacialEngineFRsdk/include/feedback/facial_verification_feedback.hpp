#ifndef FacialVerificationFeedback_INCLUDED
#define FacialVerificationFeedback_INCLUDED

#include <frsdk/verify.h>

namespace BioFacialEngine
{	
	class FacialVerificationFeedback : public FRsdk::Verification::FeedbackBody
	{
	public:
		FacialVerificationFeedback() : success_(false), score_(0.0f){}
		virtual ~FacialVerificationFeedback() {}

		void start() override{
			success_ = false;		
		}
		void end() override {}

		void processingImage(const FRsdk::Image& frame) override{}

		void eyesFound(const FRsdk::Eyes::Location& eyes) override{}
		void eyesNotFound() override {}

		void match(const FRsdk::Score& s) override	{
			score_ = s;			
			success_ = true;
		}

		void success() override{
			success_ = true;
		}

		void failure() override{
			success_ = false;
		}

		float result() const{
			return score_;
		}

	private:
		FacialVerificationFeedback(const FacialVerificationFeedback&);
		void operator=(const FacialVerificationFeedback&);

		bool  success_;	
		float score_;

	};
}
#endif
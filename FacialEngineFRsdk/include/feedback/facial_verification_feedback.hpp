#ifndef FacialVerificationFeedback_INCLUDED
#define FacialVerificationFeedback_INCLUDED

#include <frsdk/verify.h>

namespace BioFacialEngine
{
	//typedef std::function<void(const FRsdk::FIR& fir)> FirCallback;

	class FacialVerificationFeedback : public FRsdk::Verification::FeedbackBody
	{
	public:
		FacialVerificationFeedback(const BioContracts::Match match) : success_(false), match_(match) {}
		~FacialVerificationFeedback() {}

		void start() {
			success_ = false;		
		}
		void end() {}

		void processingImage(const FRsdk::Image& frame){}

		void eyesFound(const FRsdk::Eyes::Location& eyes){}
		void eyesNotFound() {}

		void match(const FRsdk::Score& s)	{
			score_ = s;			
			success_ = true;
		}

		void success(){
			success_ = true;
		}

		void failure(){
			success_ = false;
		}

		const BioContracts::Match result() const	{
			return BioContracts::Match(match_.targetFaceId(), match_.comparisonFaceId(), score_);
		}

	private:
		FacialVerificationFeedback(const FacialVerificationFeedback&);
		void operator=(const FacialVerificationFeedback&);

	private:
		BioContracts::Match match_;
		float score_;
		bool success_;
	};
}
#endif
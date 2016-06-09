#include "biotasks/verification/facial_verification_processor.hpp"
#include <ppl.h>

using namespace concurrency;
using namespace FRsdkTypes;

namespace BioFacialEngine
{

	FacialVerificationProcessor::FacialVerificationProcessor(FaceVacsConfiguration configuration) 
		                                                      : locked_(false)
	{
		parallel_invoke([&]()
		{
			try  {
				processor_ = new FRsdk::Verification::Processor(*configuration);
			}
			catch (std::exception& e) {
				std::cout << e.what();
			}
		},
			[this](){	init();	}
		);
	}


	bool FacialVerificationProcessor::locked() const {
		return locked_;
	}

	void FacialVerificationProcessor::lock()	{
		locked_ = true;
	}

	void FacialVerificationProcessor::unlock(){
		locked_ = false;
	}
	
	void FacialVerificationProcessor::init()
	{
		transformer_ = std::make_unique<VerificationProcessorTransformer>(
			[this](VerificationWorkUnitPtr pInfo) -> float
		{
			lock();
			auto score = verify(pInfo->image(), pInfo->fir());
			pInfo->done(score);
			unlock();
			return score;
		},
			nullptr,
			[this](VerificationWorkUnitPtr pInfo) -> bool  {
			return !locked();
		}
		);

	}

	float FacialVerificationProcessor::verify(FaceVacsImage image, FRsdkFir fir) const
	{
		FRsdk::SampleSet images;
		images.push_back(*image);

		FRsdk::CountedPtr<FRsdk::Score> result(new FRsdk::Score(.0f));

		FRsdk::CountedPtr<FacialVerificationFeedback> verification_feedback(new FacialVerificationFeedback());

		FRsdk::Verification::Feedback
			feedback(verification_feedback);

		processor_->process(images.begin(), images.end(), *fir, 0.01f, feedback);

		return verification_feedback->result();
	}




}
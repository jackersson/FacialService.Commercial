#include "biotasks/identification/facial_identification_processor.hpp"
#include <feedback/facial_identification_feedback.hpp>

using namespace concurrency;
using namespace FRsdkTypes;

namespace BioFacialEngine
{
	
	FacialIdentificationProcessor::FacialIdentificationProcessor( FaceVacsConfiguration configuration
			                                                        , FRsdkPopulationPtr population)
			                                                        : locked_(false)
			                                                        , processor_(nullptr)
	{
		parallel_invoke([&]()
		{
			try  {
				processor_ = new FRsdk::Identification::Processor(*configuration, *population);
			}
			catch (std::exception& e) {
				std::cout << e.what();
			}
		},
			[&]()	{	init();	}
		);
	}

	
	bool FacialIdentificationProcessor::locked() const {
		return locked_;
	}

	void FacialIdentificationProcessor::lock()	{
		locked_ = true;
	}

	void FacialIdentificationProcessor::unlock(){
		locked_ = false;
	}

		
	void FacialIdentificationProcessor::init()
	{
		transformer_ = std::make_unique<IdentificationProcessorTransformer>(
			[this](IdentificationWorkUnitPtr pInfo) -> BioContracts::MatchSet
		  {
		  	lock();
		  	auto score = identify(pInfo->image());
		  	pInfo->done(score);
		  	unlock();
		  	return score;
		  },
		  	nullptr,
		  	[this](IdentificationWorkUnitPtr pInfo) -> bool  {
		  	return !locked();
		  }
		);
	}

	BioContracts::MatchSet FacialIdentificationProcessor::identify(FRsdkTypes::FaceVacsImage image) const
	{
		FRsdk::SampleSet images;
		if (image == nullptr)
		{
			std::cout << "Image Null" << std::endl;
			return BioContracts::MatchSet();
		}

		images.push_back(*image);

		FRsdk::CountedPtr<FacialIdentificationFeedback> identification_feedback(new FacialIdentificationFeedback());

		FRsdk::Identification::Feedback
			feedback(identification_feedback);

	  processor_->process(images.begin(), images.end(), 0.01f, feedback, MAX_MATCHES_COUNT);
		
		return BioContracts::MatchSet(identification_feedback->result());
	}
}
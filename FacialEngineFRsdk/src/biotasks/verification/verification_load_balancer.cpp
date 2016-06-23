#include "biotasks/verification/verification_load_balancer.hpp"
#include "ppl.h"
#include <future>

using namespace concurrency;
using namespace FRsdkTypes;

namespace BioFacialEngine
{

	VerificationLoadBalancer::VerificationLoadBalancer( FaceVacsConfiguration configuration
		                                                , int count )
		                                                : PipelineGovernor(0)
		                                                , configuration_(configuration)
	{
		init(count);
	}


	void VerificationLoadBalancer::resize(int count) {
		init(count);
	}

	float VerificationLoadBalancer::verify(FaceVacsImage image, FRsdkFir fir)
	{
		VerificationWorkUnitPtr unit(new VerificationWorkUnit(image, fir));
		wait_available_working_unit();
		asend(inputs_, unit);
		unit->wait();
		free_unit();
		return unit->score();
	}

	void VerificationLoadBalancer::stop()	{
		wait_until_empty();
	}

	void VerificationLoadBalancer::init(int count)
	{
		auto available_count = MAX_PROCESSORS_COUNT - capacity_;
		if (available_count <= 0 || available_count <= count)
		{
			std::cout << "max count of verification load balancer reached" << std::endl;
			return;
		}

		parallel_for(0, MAX_PROCESSORS_COUNT, 1,
			[&](int i)
		{
			FacialVerificationProcessorPtr ptr(new FacialVerificationProcessor(configuration_));
			if (ptr != nullptr)
			{
				ptr->link_to<VerificationWorkUnitPtr>(inputs_);
				processors_.push_back(ptr);
			}
		});
		PipelineGovernor::resize(processors_.size());
	}	
}
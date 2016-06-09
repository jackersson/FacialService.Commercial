#include "biotasks/enrollment/enrollment_load_balancer.hpp"

#include <ppltasks.h>

using namespace concurrency;
using namespace FRsdkTypes ;

namespace BioFacialEngine
{
	
	EnrollmentLoadBalancer::EnrollmentLoadBalancer( FaceVacsConfiguration configuration
		                                            , int count)
		                                            : PipelineGovernor(0)
		                                            , configuration_(configuration)
	{
		init(count);
	}

	void EnrollmentLoadBalancer::resize(int count) {
		init(count);
	}

	void EnrollmentLoadBalancer::enroll(FaceVacsImage samples, FRsdk::Enrollment::Feedback& feedback)
	{
		FRsdkEnrollmentWorkUnitPtr unit(new FRsdkEnrollmentWorkUnit(samples, feedback));
		wait_available_working_unit();
		asend(inputs_, unit);
		unit->wait();
		free_unit();
	}

	void EnrollmentLoadBalancer::stop()	{
		wait_until_empty();
	}
		
	void EnrollmentLoadBalancer::init(int count)
	{
		auto available_count = MAX_PROCESSORS_COUNT - capacity_;
		if (available_count <= 0 || available_count <= count)
		{
			std::cout << "max count of enrollment load balancer reached" << std::endl;
			return;
		}

		parallel_for(0, MAX_PROCESSORS_COUNT, count,
			[&](int i)
		{
			FRsdkAsyncEnrollmentProcessorPtr ptr(new FRsdkEnrollmentProcessor(configuration_));
			if (ptr != nullptr)
			{
				ptr->link_to<FRsdkEnrollmentWorkUnitPtr>(inputs_);
				processors_.push_back(ptr);
			}
		}
		);
		PipelineGovernor::resize(processors_.size());
	}		
}

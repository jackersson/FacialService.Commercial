#include "biotasks/identification/identification_load_balancer.hpp"

using namespace concurrency;
using namespace FRsdkTypes;

namespace BioFacialEngine
{

	IdentificationLoadBalancer::IdentificationLoadBalancer( FaceVacsConfiguration configuration
			                                                  , FRsdkPopulationPtr population
			                                                  , int count)
			                                                  : PipelineGovernor(0)
			                                                  , configuration_(configuration)
			                                                  , population_(population)
	{
		//count = 1;
		init(1);
	}

	void IdentificationLoadBalancer::resize(int count)  {
		init(count);
	}


	BioContracts::MatchSet IdentificationLoadBalancer::identify(FaceVacsImage image)
	{
		auto unit(std::make_shared<IdentificationWorkUnit>(image));
		wait_available_working_unit();
		concurrency::asend(inputs_, unit);
		unit->wait();
		free_unit();
		return unit->score();
	}

	void IdentificationLoadBalancer::stop()	{
		wait_until_empty();
	}

	void IdentificationLoadBalancer::init(int count)
	{
		std::cout << "IdentificationLoadBalancer::init 1 " << std::endl;
		auto available_count = MAX_PROCESSORS_COUNT - capacity_;
		if (available_count <= 0 || available_count <= count)
		{
			std::cout << "max count of verification load balancer reached" << std::endl;
			return;
		}
		std::cout << "IdentificationLoadBalancer::init 2 " << std::endl;
		parallel_for(0, count, 1,
			[&](int i)
		  {
		  	auto ptr(std::make_shared<FacialIdentificationProcessor>(configuration_, population_));
		  	if (ptr != nullptr)
		  	{
		  		ptr->link_to<IdentificationWorkUnitPtr>(inputs_);
		  		processors_.push_back(ptr);
		  	}
		  }
		);
		std::cout << "IdentificationLoadBalancer::init 3 " << std::endl;
		PipelineGovernor::resize(processors_.size());
		std::cout << "IdentificationLoadBalancer::init 4 " << std::endl;
	}		
}
#ifndef IdentificationLoadBalancer_INCLUDED
#define IdentificationLoadBalancer_INCLUDED

#include <utils/pipeline_governor.hpp>
#include "facial_identification_processor.hpp"

namespace BioFacialEngine
{

	class IdentificationLoadBalancer : public Pipeline::PipelineGovernor
	{
	public:
		explicit IdentificationLoadBalancer(FRsdkTypes::FaceVacsConfiguration configuration
			                                , FRsdkPopulationPtr population
			                                , int count = MIN_PROCESSORS_COUNT);			                                

		virtual ~IdentificationLoadBalancer() {}

		void resize(int count) override;


		BioContracts::MatchSet identify(FRsdkTypes::FaceVacsImage image);

		void stop();

	private:
		void init(int count);

		IdentificationLoadBalancer(const IdentificationLoadBalancer&);
		IdentificationLoadBalancer const & operator=(IdentificationLoadBalancer const&);
		
		FRsdkTypes::FaceVacsConfiguration configuration_;
		FRsdkPopulationPtr                population_;

		concurrency::unbounded_buffer<IdentificationWorkUnitPtr>  inputs_;
		std::list<FacialIdentificationProcessorPtr>               processors_;

		static const int MIN_PROCESSORS_COUNT = 1;
		static const int MAX_PROCESSORS_COUNT = 3;
	};

	typedef std::shared_ptr<IdentificationLoadBalancer> IdentificationLoadBalancerPtr;
}
#endif

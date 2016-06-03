#ifndef VerificationLoadBalancecr_INCLUDED
#define VerificationLoadBalancecr_INCLUDED

//#include "biotasks\ifir_builder.hpp"
//#include "common\matches.hpp"
//#include <wrappers/face_info.hpp>
#include <utils/pipeline_governor.hpp>
//#include "verification_work_unit.hpp"

#include "facial_verification_processor.hpp"


namespace BioFacialEngine
{
	class VerificationLoadBalancer : public Pipeline::PipelineGovernor
	{
	public:
		explicit VerificationLoadBalancer(FRsdkTypes::FaceVacsConfiguration configuration
		                               	, int count = MIN_PROCESSORS_COUNT);			

		virtual ~VerificationLoadBalancer() {}

		void  resize(int count) override;

		float verify(FRsdkTypes::FaceVacsImage image, FRsdkFir fir);
		void  stop();

	private:
		void init(int count);

		VerificationLoadBalancer(const VerificationLoadBalancer&);
		VerificationLoadBalancer const & operator=(VerificationLoadBalancer const&);
		
		FRsdkTypes::FaceVacsConfiguration configuration_;

		concurrency::unbounded_buffer<VerificationWorkUnitPtr>  inputs_;
		std::list<FacialVerificationProcessorPtr>                processors_;

		static const int MIN_PROCESSORS_COUNT = 1;
		static const int MAX_PROCESSORS_COUNT = 3;
	};

	typedef std::shared_ptr<VerificationLoadBalancer> VerificationLoadBalancerPtr;
}
#endif
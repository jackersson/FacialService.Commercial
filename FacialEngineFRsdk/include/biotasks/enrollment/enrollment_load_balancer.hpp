#ifndef EnrollmentLoadBalancer_INCLUDED
#define EnrollmentLoadBalancer_INCLUDED

//#include "utils/face_vacs_includes.hpp"
//#include "feedback/facial_enrollment_feedback.hpp"

#include "utils\pipeline_governor.hpp"

#include "frsdk_enrollment_work_unit.hpp"
#include "frsdk_enrollment_processor.hpp"

namespace BioFacialEngine
{	
	class EnrollmentLoadBalancer : public Pipeline::PipelineGovernor
	{
	public:
		explicit EnrollmentLoadBalancer( FRsdkTypes::FaceVacsConfiguration configuration
			                             , int count = MIN_PROCESSORS_COUNT);			                             

		void resize(int count) override;
		void enroll(FRsdkTypes::FaceVacsImage samples, FRsdk::Enrollment::Feedback& feedback);		
		void stop();


	private:
		void init(int count);

		EnrollmentLoadBalancer(const EnrollmentLoadBalancer&);
		EnrollmentLoadBalancer const & operator=(EnrollmentLoadBalancer const&);
				
		FRsdkTypes::FaceVacsConfiguration configuration_;

		concurrency::unbounded_buffer<FRsdkEnrollmentWorkUnitPtr>  inputs_;
		std::list<FRsdkAsyncEnrollmentProcessorPtr>                processors_;

		static const int MIN_PROCESSORS_COUNT = 1;
		static const int MAX_PROCESSORS_COUNT = 3;
	};

	typedef std::shared_ptr<EnrollmentLoadBalancer> EnrollmentLoadBalancerPtr;
}
#endif
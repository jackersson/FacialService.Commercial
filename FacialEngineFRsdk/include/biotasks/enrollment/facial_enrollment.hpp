#ifndef FacialEnrollment_INCLUDED
#define FacialEnrollment_INCLUDED

#include "utils/face_vacs_includes.hpp"
#include "feedback/facial_enrollment_feedback.hpp"

#include "enrollment_load_balancer.hpp"

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Enrollment::Processor> FRsdkEnrollmentProcessorPtr;	
		
	class FacialEnrollment final 
	{
	public:
		explicit FacialEnrollment(const std::string& configuration_filename);
		explicit FacialEnrollment(FRsdkTypes::FaceVacsConfiguration configuration);
		
		void enroll(FRsdkTypes::FaceVacsImage image, FRsdk::Enrollment::Feedback& feedback) const;	

	private:
		FacialEnrollment(const FacialEnrollment&);
		FacialEnrollment const & operator=(FacialEnrollment const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

		EnrollmentLoadBalancerPtr load_balancer_;
	
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};

	typedef std::shared_ptr<FacialEnrollment> FacialEnrollmentPtr;
}

#endif
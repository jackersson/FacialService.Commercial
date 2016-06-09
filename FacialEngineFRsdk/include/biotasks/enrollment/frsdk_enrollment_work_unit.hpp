#ifndef FRsdkEnrollmentWorkUnit_INCLUDED
#define FRsdkEnrollmentWorkUnit_INCLUDED

#include "utils/face_vacs_includes.hpp"
#include "feedback/facial_enrollment_feedback.hpp"
#include <agents.h>

namespace BioFacialEngine
{	
	typedef std::pair<FRsdkTypes::FaceVacsImage, FRsdk::Enrollment::Feedback> FRsdkEnrollmentContext;

	class FRsdkEnrollmentWorkUnit
	{
	public:
		explicit FRsdkEnrollmentWorkUnit(FRsdkTypes::FaceVacsImage samples
			, const FRsdk::Enrollment::Feedback& feedback)
			: enrollment_data_(samples, feedback)
		{}

		FRsdkTypes::FaceVacsImage samples() const	{
			return enrollment_data_.first;
		}

		FRsdk::Enrollment::Feedback& feedback() {
			return enrollment_data_.second;
		}

		void done() {
			asend(done_, true);
		}

		void wait() {
			receive(&done_);
		}

	private:
		FRsdkEnrollmentWorkUnit(const FRsdkEnrollmentWorkUnit&);
		FRsdkEnrollmentWorkUnit const & operator=(FRsdkEnrollmentWorkUnit const&);

		concurrency::single_assignment<bool> done_;
		FRsdkEnrollmentContext enrollment_data_;
	};
	typedef std::shared_ptr<FRsdkEnrollmentWorkUnit> FRsdkEnrollmentWorkUnitPtr;
}

#endif
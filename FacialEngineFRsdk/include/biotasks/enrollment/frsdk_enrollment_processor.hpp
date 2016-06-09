#ifndef FRsdkEnrollmentProcessor_INCLUDED
#define FRsdkEnrollmentProcessor_INCLUDED

#include <mutex>
#include "frsdk_enrollment_work_unit.hpp"
#include <agents.h>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Enrollment::Processor> FRsdkEnrollmentProcessorPtr;

	class FRsdkEnrollmentProcessor
	{
		typedef Concurrency::transformer<FRsdkEnrollmentWorkUnitPtr, bool> EnrollmentProcessorTransformer;
	public:
		explicit FRsdkEnrollmentProcessor(FRsdkTypes::FaceVacsConfiguration configuration);                 
	
		bool locked() const {
			return locked_;
		}

		void lock()	{
			locked_ = true;
		}

		void unlock(){
			locked_ = false;
		}

		template<typename T>
		void link_to(Concurrency::unbounded_buffer<T>& buffer) const	{
			buffer.link_target(transformer_.get());
		}

	private:
		bool enroll(FRsdkTypes::FaceVacsImage image, FRsdk::Enrollment::Feedback& feedback);
		void init();

		FRsdkEnrollmentProcessor(const FRsdkEnrollmentProcessor&);
		FRsdkEnrollmentProcessor const & operator=(FRsdkEnrollmentProcessor const&);

		bool       locked_;
		std::mutex mutex_;

		std::unique_ptr<EnrollmentProcessorTransformer> transformer_;

		FRsdkEnrollmentProcessorPtr processor_;
	};

	typedef std::shared_ptr<FRsdkEnrollmentProcessor> FRsdkAsyncEnrollmentProcessorPtr;
}
#endif
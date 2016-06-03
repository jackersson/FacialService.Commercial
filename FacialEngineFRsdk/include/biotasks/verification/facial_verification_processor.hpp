#ifndef FacialVerificationProcessor_INCLUDED
#define FacialVerificationProcessor_INCLUDED

#include "feedback\facial_verification_feedback.hpp"
#include "verification_work_unit.hpp"
#include "biotasks/ifir_builder.hpp"

#include <mutex>
#include <agents.h>

namespace BioFacialEngine
{	
	typedef FRsdk::CountedPtr<FRsdk::Verification::Processor> FRsdkVerificationProcessor;

	class FacialVerificationProcessor
	{
		typedef Concurrency::transformer<VerificationWorkUnitPtr, float> VerificationProcessorTransformer;

	public:
		explicit FacialVerificationProcessor(FRsdkTypes::FaceVacsConfiguration configuration);
	
		~FacialVerificationProcessor(){}

		bool locked() const;
		void lock  ();
		void unlock();

		template<typename T>
		void link_to(Concurrency::unbounded_buffer<T>& buffer) const	{
			buffer.link_target(transformer_.get());
		}

	private:
		void init();

		float verify(FRsdkTypes::FaceVacsImage image, FRsdkFir fir) const;

		FacialVerificationProcessor(const FacialVerificationProcessor&);
		FacialVerificationProcessor const & operator=(FacialVerificationProcessor const&);

		FRsdkVerificationProcessor processor_;

		bool locked_;
		std::mutex mutex_;

		std::unique_ptr<VerificationProcessorTransformer> transformer_;

		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
		const float FAR_THRESHOLD = 0.001f;
	};

	typedef std::shared_ptr<FacialVerificationProcessor> FacialVerificationProcessorPtr;
}
#endif
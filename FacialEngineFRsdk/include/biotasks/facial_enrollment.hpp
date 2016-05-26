#ifndef FacialEnrollment_INCLUDED
#define FacialEnrollment_INCLUDED

#include "utils/face_vacs_includes.hpp"
#include "feedback/facial_enrollment_feedback.hpp"

#include "utils\pipeline_governor.hpp"
#include <ppltasks.h>
#include <mutex>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Enrollment::Processor> FRsdkEnrollmentProcessorPtr;	

  typedef std::pair<FRsdkTypes::FaceVacsImage, FRsdk::Enrollment::Feedback> FRsdkEnrollmentContext;
	
	class FRsdkEnrollmentWorkUnit
	{
	public: 
		explicit FRsdkEnrollmentWorkUnit( FRsdkTypes::FaceVacsImage samples
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

	
	class FRsdkEnrollmentProcessor
	{
		typedef Concurrency::transformer<FRsdkEnrollmentWorkUnitPtr, bool> EnrollmentProcessorTransformer;
	public: 
		explicit FRsdkEnrollmentProcessor( FRsdkTypes::FaceVacsConfiguration configuration )
			                               : locked_(false)
													           , transformer_(nullptr)
		{
			concurrency::parallel_invoke
			([&]()
			  {
			  	try  {			  	
			    	processor_ = new FRsdk::Enrollment::Processor(*configuration);			  	 
			    }
			    catch (std::exception& e) {
			  	  std::cout << e.what();
		    	}				
		    },
			  [this](){	init();	}
			);		
		}

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
		bool enroll(FRsdkTypes::FaceVacsImage image, FRsdk::Enrollment::Feedback& feedback)
		{
			std::lock_guard<std::mutex> lock_object(mutex_);
			
			auto flag(false);
			try
			{			
				
				FRsdk::SampleSet images;
				images.push_back(*image);
				auto start = clock();
				processor_->process(images.begin(), images.end(), feedback);
				std::cout << " full enrollment  " << clock() - start << std::endl;
				flag = true;
			}
			catch (std::exception& e) {
				std::cout << e.what();					
			}
			
			return flag;
		}	

		void init()
		{			
			transformer_ = std::make_unique<EnrollmentProcessorTransformer>(
				[this](FRsdkEnrollmentWorkUnitPtr pInfo)-> bool
			  {
					lock();
					auto state = enroll(pInfo->samples(), pInfo->feedback());
					pInfo->done();
					unlock();
					return state;
			  },
			  nullptr,
				[this](FRsdkEnrollmentWorkUnitPtr pInfo)->bool  {				 
					return !locked();
			  }
			);
			
		}

		FRsdkEnrollmentProcessor(const FRsdkEnrollmentProcessor&);
		FRsdkEnrollmentProcessor const & operator=(FRsdkEnrollmentProcessor const&);
	
		bool       locked_;
		std::mutex mutex_ ;

		std::unique_ptr<EnrollmentProcessorTransformer> transformer_;

		FRsdkEnrollmentProcessorPtr processor_;
	};

	typedef std::shared_ptr<FRsdkEnrollmentProcessor> FRsdkAsyncEnrollmentProcessorPtr;

	class EnrollmentLoadBalancer : public Pipeline::PipelineGovernor
	{
	public:
		explicit EnrollmentLoadBalancer( FRsdkTypes::FaceVacsConfiguration configuration
			                             , int count = MIN_PROCESSORS_COUNT    )
			                             : Pipeline::PipelineGovernor(0)
																	 , configuration_(configuration)											        
		{
			init(count);		
		}		

		void resize(int count) override {
			init(count);
		}		

		void enroll(FRsdkTypes::FaceVacsImage samples, FRsdk::Enrollment::Feedback& feedback)
		{							
			FRsdkEnrollmentWorkUnitPtr unit(new FRsdkEnrollmentWorkUnit(samples, feedback));
			wait_available_working_unit();
			concurrency::asend(inputs_, unit);
			unit->wait();	
			free_unit();
		}

		void stop()	{
			wait_until_empty();
		}

	private:
		EnrollmentLoadBalancer(const EnrollmentLoadBalancer&);
		EnrollmentLoadBalancer const & operator=(EnrollmentLoadBalancer const&);

		void init(int count)
		{
			auto available_count = MAX_PROCESSORS_COUNT - capacity_;
			if (available_count <= 0 || available_count <= count)
			{
				std::cout << "max count of enrollment load balancer reached" << std::endl;
				return;
			}

			concurrency::parallel_for(0, MAX_PROCESSORS_COUNT, count,
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
			Pipeline::PipelineGovernor::resize(processors_.size());
		}
		FRsdkTypes::FaceVacsConfiguration configuration_;

		concurrency::unbounded_buffer<FRsdkEnrollmentWorkUnitPtr>  inputs_     ;
		std::list<FRsdkAsyncEnrollmentProcessorPtr>                processors_ ;

		static const int MIN_PROCESSORS_COUNT = 1;
		static const int MAX_PROCESSORS_COUNT = 3;
	};

	typedef std::shared_ptr<EnrollmentLoadBalancer> EnrollmentLoadBalancerPtr;

	class FacialEnrollmentProcessor final //: public IEnrollmentProcessor
	{
	public:
		explicit FacialEnrollmentProcessor(const std::string& configuration_filename);
		explicit FacialEnrollmentProcessor(FRsdkTypes::FaceVacsConfiguration configuration);
		
		void enroll(FRsdkTypes::FaceVacsImage image, FRsdk::Enrollment::Feedback& feedback) const
		{			
			load_balancer_->enroll(image, feedback);
		}

	private:
		FacialEnrollmentProcessor(const FacialEnrollmentProcessor&);
		FacialEnrollmentProcessor const & operator=(FacialEnrollmentProcessor const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

		EnrollmentLoadBalancerPtr load_balancer_;
	
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};

	typedef std::shared_ptr<FacialEnrollmentProcessor> FacialEnrollmentPtr;
}

#endif
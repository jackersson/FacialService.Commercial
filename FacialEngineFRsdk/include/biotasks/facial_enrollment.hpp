#ifndef FacialEnrollment_INCLUDED
#define FacialEnrollment_INCLUDED

#include "biotasks/ienrollment_processor.hpp"
#include "biotasks/ienrollment_able.hpp"
#include "utils/face_vacs_includes.hpp"
#include "feedback/facial_enrollment_feedback.hpp"

#include "utils\work_unit_governor.hpp"
#include <ppltasks.h>
#include <mutex>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Enrollment::Processor> FRsdkEnrollmentProcessorPtr;	

	typedef std::pair<FRsdk::SampleSet, FRsdk::Enrollment::Feedback> FRsdkEnrollmentUnit;
	

	class FRsdkEnrollmentTask
	{
	public: 
		FRsdkEnrollmentTask( const FRsdk::SampleSet& samples, const FRsdk::Enrollment::Feedback& feedback)
		                  	: enrollment_data_(samples, feedback)
		{}

		const FRsdk::SampleSet& samples() const		{
			return enrollment_data_.first;
		}

		FRsdk::Enrollment::Feedback& feedback() 		{
			return enrollment_data_.second;
		}

		void done(){
			asend(done_, true);
		}

		void wait(){
			receive(&done_);
		}

	private:
		concurrency::single_assignment<bool> done_;
		FRsdkEnrollmentUnit enrollment_data_;
	};

	typedef std::shared_ptr<FRsdkEnrollmentTask> FRsdkEnrollmentUnitPtr;

	class EnrollmentProcessorUnit
	{
	public: 
		EnrollmentProcessorUnit( FaceVacsConfiguration configuration )
			                     : locked_(false)
													 , transformer_(nullptr)
		{
			bool is_ok = false;
			try
			{
				enrollment_processor_ = new FRsdk::Enrollment::Processor(*configuration);
				is_ok = true;
				
				transformer_ = std::unique_ptr<concurrency::transformer<FRsdkEnrollmentUnitPtr, bool>>(
					new concurrency::transformer<FRsdkEnrollmentUnitPtr, bool>(
					[this](FRsdkEnrollmentUnitPtr pInfo)-> bool
				  {
						lock();
						enroll(pInfo->samples(), pInfo->feedback());
						pInfo->done();
						unlock();
					  return true;
				  },
				  nullptr,
					[this](FRsdkEnrollmentUnitPtr pInfo)->bool
				  {				 
						return !locked();
				  }
					));
			}	
			catch (std::exception& e) {
				std::cout << e.what();
			}

			if (!is_ok)
				this->~EnrollmentProcessorUnit();
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

		void link_to(concurrency::unbounded_buffer<FRsdkEnrollmentUnitPtr>& buffer)
		{
			buffer.link_target(transformer_.get());
		}
	
	private:
		bool enroll(const FRsdk::SampleSet& images, FRsdk::Enrollment::Feedback& feedback)
		{
			std::lock_guard<std::mutex> lock_object(mutex_);
			
			bool flag(false);
			try
			{			
				clock_t start = clock();
				enrollment_processor_->process(images.begin(), images.end(), feedback);
				std::cout << " full enrollment  " << clock() - start << std::endl;
				flag = true;
			}
			catch (std::exception& e) {
				std::cout << e.what();					
			}
			
			return flag;
		}
	
	private:
		EnrollmentProcessorUnit(const EnrollmentProcessorUnit&);
		EnrollmentProcessorUnit const & operator=(EnrollmentProcessorUnit const&);

	private:
		bool locked_;
		std::mutex mutex_;
		std::unique_ptr<concurrency::transformer<FRsdkEnrollmentUnitPtr, bool>> transformer_;
		FRsdkEnrollmentProcessorPtr enrollment_processor_;
	};

	typedef std::shared_ptr<EnrollmentProcessorUnit> FRsdkEnrollmentProcessorUnitPtr;

	class EnrollmentProcessorPool : public Pipeline::WorkUnitGovernor
	{
	public:
		EnrollmentProcessorPool( FaceVacsConfiguration configuration ) 
			                     : configuration_(configuration)
													 , Pipeline::WorkUnitGovernor(MAX_PROCESSORS_COUNT)
		{
			init();
		}		

		void init()
		{
			concurrency::parallel_for(0, MAX_PROCESSORS_COUNT, 1,
				[&]( int i)
			  {
				  FRsdkEnrollmentProcessorUnitPtr ptr(new EnrollmentProcessorUnit(configuration_));
				  if (ptr != nullptr)
				  {
				  	ptr->link_to(inputs_);
				  	units_.push_back(ptr);
				  }
			  }
			);
		}

		bool enroll(const FRsdk::SampleSet& images, const FRsdk::Enrollment::Feedback& feedback)
		{				
			FRsdkEnrollmentUnitPtr ptr(new FRsdkEnrollmentTask(images, feedback));
			AddWorkingUnit();
			concurrency::asend(inputs_, ptr);
			ptr->wait();
			return true;
		}

		void stop()
		{
			Wait();
		}


	private:
		FaceVacsConfiguration configuration_;

		concurrency::unbounded_buffer<FRsdkEnrollmentUnitPtr>  inputs_ ;
		std::list<FRsdkEnrollmentProcessorUnitPtr> units_;

		static const int MIN_PROCESSORS_COUNT = 2;
		static const int MAX_PROCESSORS_COUNT = 3;
	};

	typedef std::shared_ptr<EnrollmentProcessorPool> EnrollmentProcessorPoolPtr;

	class FacialEnrollment //: public IEnrollmentProcessor
	{
	public:
		FacialEnrollment(const std::string& configuration_filename);
		FacialEnrollment(FaceVacsConfiguration configuration);
		~FacialEnrollment(){}
		
		bool enroll(const FRsdk::Image& image, FRsdk::Enrollment::Feedback& feedback)
		{
			FRsdk::SampleSet images;
			images.push_back(image);
			return pool_->enroll(images, feedback);			
		}

		FRsdkFir build(const std::string& bytes);

	private:
		bool init(FaceVacsConfiguration configuration);

	private:
		EnrollmentProcessorPoolPtr pool_;
		//FRsdkEnrollmentProcessorPtr processor_  ;
		FirBuilderRef               fir_builder_;
	private:
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};

	typedef std::shared_ptr<FacialEnrollment> FacialEnrollmentPtr;
}

#endif
#include "biotasks/enrollment/frsdk_enrollment_processor.hpp"

#include <ppl.h>

using namespace concurrency;
using namespace FRsdkTypes;
namespace BioFacialEngine
{	

	FRsdkEnrollmentProcessor::FRsdkEnrollmentProcessor( FaceVacsConfiguration configuration)
			                                              : locked_(false)
			                                              , transformer_(nullptr)
	{
		parallel_invoke([&]()
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
	
	bool FRsdkEnrollmentProcessor::enroll(FaceVacsImage image, FRsdk::Enrollment::Feedback& feedback)
	{
		std::lock_guard<std::mutex> lock_object(mutex_);

		auto flag(false);
		try
		{
			FRsdk::SampleSet images;
			images.push_back(*image);			
			processor_->process(images.begin(), images.end(), feedback);		
			flag = true;
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}

		return flag;
	}

	void FRsdkEnrollmentProcessor::init()
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
	
}

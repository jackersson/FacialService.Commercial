#ifndef FacialEngine_INCLUDED
#define FacialEngine_INCLUDED

#include "engine/ifacial_engine.hpp"
#include <string>
#include "pipeline/frsdk_biometric_processor.hpp"
#include "pipeline/biometric_pipeline_load_balanced.hpp"

namespace BioFacialEngine
{

	class FacialEngine : public BioContracts::IFacialEngine
	{
	public:
		explicit FacialEngine(const std::string& frsdk_configuration);
		virtual ~FacialEngine() {}

		
		BioContracts::IImageInfoPtr
		acquire(const std::string& filename) override
		{
			auto acquire = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;
			std::string shwarc = "C:\\Users\\jacke\\Desktop\\3423.jpg";
			return pipeline_->acquire(shwarc, acquire);
		} 

		BioContracts::IImageInfoPtr
		acquire(const BioContracts::RawImage& raw_image) override
		{
			auto acquire = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;

			//clock_t start = clock();
			auto ptr = pipeline_->acquire(raw_image, acquire);
			//std::cout << "elapsed ticks" << clock() - start << std::endl;
			return ptr;
		}
		
		/*
		BioContracts::IImageInfoPtr
			enroll(const std::string& filename) override
		{
			std::string shwarc = "C:\\Users\\jacke\\Desktop\\3423.jpg";
			FRsdkEntities::ImageInfoPtr ptr(new FRsdkEntities::ImageInfo(shwarc));
			return ptr;
		}
		

		BioContracts::IImageInfoPtr
			enroll(const BioContracts::RawImage& raw_image) override
		{
			std::string shwarc = "C:\\Users\\jacke\\Desktop\\3423.jpg";
			FRsdkEntities::ImageInfoPtr ptr(new FRsdkEntities::ImageInfo(shwarc));
			return ptr;
		}
		*/

	private:
		FacialEngine(const FacialEngine&);

		//std::unique_ptr<FacialAcquisition>  acquisition_ ;
		//std::unique_ptr<FacialEnrollmentProcessor>   enrollment_  ;
		//std::unique_ptr<FacialVerification> verification_;
		std::unique_ptr<Pipeline::BiometricPipelineBalanced> pipeline_;
		Pipeline::FRsdkBiometricProcessorPtr processor_;

	};

	typedef std::shared_ptr<BioFacialEngine::FacialEngine> FacialEnginePtr;
}

#endif
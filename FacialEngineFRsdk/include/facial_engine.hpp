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
			return pipeline_->acquire(filename, acquire);
		} 

		BioContracts::IImageInfoPtr
		acquire(const BioContracts::RawImage& raw_image) override
		{
			auto acquire = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;
			auto ptr = pipeline_->acquire(raw_image, acquire);		
			return ptr;
		}

		BioContracts::VerificationResultPtr
		verify(const std::string& object, const std::string& subject) override
		{
			//auto verify = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;		
			return pipeline_->verify_face(object, subject);
		}

		BioContracts::VerificationResultPtr
		verify(const BioContracts::RawImage& object, const BioContracts::RawImage& subject) override
		{
			//auto verify = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;
			return pipeline_->verify_face(object, subject);
		}

		BioContracts::IdentificationResultPtr
		identify(const std::string& object, const std::list<std::string>& subject) override
		{
			//auto verify = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;		
			return pipeline_->identify_face(object, subject);
		}

		BioContracts::IdentificationResultPtr
		identify(const BioContracts::RawImage& object, const std::list<BioContracts::RawImage>& subject) override
		{
			//auto verify = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;
			return pipeline_->identify_face(object, subject);
		}

		BioContracts::IdentificationResultPtr
		identify(const BioContracts::RawImage& object, long population_id ) override
		{
		
			//auto verify = Pipeline::BimetricMultiTask::FULL_PORTRAIT_ANALYSIS;
			return pipeline_->identify_face(object, population_id);
		}

		BioContracts::IImageInfoSet
		create_population(const std::list<BioContracts::RawImage>& subjects, long& population_id) override	{

			auto images = pipeline_->load_identification_population(subjects, population_id);
			BioContracts::IImageInfoSet image_info_set;

			image_info_set.set_identification_images<std::list<FRsdkEntities::ImageInfoPtr>::iterator>
				(images.begin(), images.end());
		
			return image_info_set;
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
#ifndef FRsdkBiometricProcessor_INCLUDED
#define FRsdkBiometricProcessor_INCLUDED

#include "pipeline/ibiometric_processor.hpp"
#include "utils/face_vacs_includes.hpp"
#include <ctime>

#include "biotasks/facial_acquisition.hpp"
#include "biotasks/facial_enrollment.hpp"
#include "biotasks/facial_verification.hpp"

namespace Pipeline
{
	class FRsdkBiometricProcessor : public IBiometricProcessor
	{
	public:
		explicit
		FRsdkBiometricProcessor(const std::string& configuration_filename)
		{			
			try	{
				FRsdkTypes::FaceVacsConfiguration configuration(new FRsdk::Configuration(configuration_filename));
				init(configuration);
			}		
			catch (std::exception& e) {
				std::cout << e.what();
			}
		}

		explicit
		FRsdkBiometricProcessor(FRsdkTypes::FaceVacsConfiguration configuration)
		{
			init(configuration);
		}

		bool init(FRsdkTypes::FaceVacsConfiguration configuration)
		{
			try
			{
				Concurrency::parallel_invoke
				(
				  [&]() 
				  {
						acquisition_ = std::make_shared<BioFacialEngine::FacialAcquisition>(configuration);				  		      ;		         
				  },
				  [&]()
				  {
				  	enrollment_ = std::make_shared<BioFacialEngine::FacialEnrollmentProcessor>(configuration);				  			  
				  },
				  [&]() 
				  {
						veryfier_ = std::make_shared<BioFacialEngine::FacialVerification>(configuration);				  		                      
				  },
				  	[&]()
				  {
						fir_builder_ = std::make_shared<BioFacialEngine::FirBilder>(configuration);				  			                           	
				  }
				);
				return true;
			}
			catch (const FRsdk::FeatureDisabled& e) { std::cout << e.what(); return false; }
			catch (const FRsdk::LicenseSignatureMismatch& e) { std::cout << e.what(); return false; }
			catch (std::exception& e) { std::cout << e.what(); return false; }
		}

		FRsdkEntities::ImageInfoPtr load_image(const std::string& filename) override
		{
			FRsdkEntities::ImageInfoPtr image(new FRsdkEntities::ImageInfo(filename));
			return image;
		}

		void face_find(FRsdkEntities::ImageInfoPtr pInfo) override
		{
			auto image = pInfo->image();

			std::vector<FRsdkEntities::FaceVacsFullFace> faces;
			acquisition_->findFace(image, faces);

			pInfo->addRange(faces);
		}

		void portrait_analyze(FRsdkEntities::FaceInfoPtr pInfo) override
		{
			auto pch = acquisition_->analyze(pInfo->annotated_image());
			FRsdkEntities::FRsdkFaceCharacteristicPtr portrait(new FRsdkEntities::FRsdkFaceCharacteristic(*pch));
			pInfo->set_portrait_characteristics(portrait);
		}

		void extract_facial_image(FRsdkEntities::FaceInfoPtr pInfo) override
		{
			auto result = acquisition_->extractFace(pInfo->annotated_image());
			pInfo->set_facial_image(result);

			//std::cout << "extraction stage " << clock() << std::endl;

		}

		void iso_compliance_test(FRsdkEntities::FaceInfoPtr pInfo) override {
			auto portrait = pInfo->characteristics().portraitCharacteristics();
			auto result = acquisition_->isoComplianceTest(portrait);

			FRsdkEntities::FRsdkIsoCompliancePtr iso(new FRsdkEntities::FRsdkIsoCompliance(*result));
			pInfo->set_iso_compliance(iso);
		}

		void enroll(FRsdkEntities::FaceInfoPtr pInfo) override
		{		
			std::cout << "enrollment stage " << pInfo->id() << " " << clock() << std::endl;

			FRsdk::Enrollment::Feedback
				feedback((pInfo->enrollment_data()));

			enrollment_->enroll(pInfo->extracted_image(), feedback);

			std::cout << "enrollment stage finnished " << pInfo->id() << " " << clock() << std::endl;
		}


		void verify(FRsdkEntities::FaceInfoPtr pInfo) override
		{
			//veryfier_->Add(pInfo);
		}

	private:
		BioFacialEngine::FaceVacsAcquisitionPtr     acquisition_;
		BioFacialEngine::FacialEnrollmentPtr        enrollment_ ;	
		BioFacialEngine::FacialVerificationPtr      veryfier_   ;
		BioFacialEngine::FirBuilderRef              fir_builder_;

		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};

	typedef std::shared_ptr<FRsdkBiometricProcessor> FRsdkBiometricProcessorPtr;
}
#endif

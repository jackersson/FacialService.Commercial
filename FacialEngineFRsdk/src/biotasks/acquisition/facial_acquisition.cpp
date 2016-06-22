#include <biotasks/acquisition/facial_acquisition.hpp>

using namespace concurrency;
using namespace FRsdkTypes;

namespace BioFacialEngine
{
	FacialAcquisition::FacialAcquisition(const std::string& configuration_filename)
	{		
		try
		{
			auto configuration (std::make_shared<FRsdk::Configuration>(configuration_filename));
		  init(configuration);
	  }
	  catch (std::exception& e) {
			std::cout << e.what();		
		}
	}

	FacialAcquisition::FacialAcquisition(FaceVacsConfiguration configuration)
	{
		init(configuration);			
	}

	bool FacialAcquisition::init(FaceVacsConfiguration configuration)
	{
		try
		{
			concurrency::parallel_invoke(
				[&](){ face_finder_        = new FRsdk::Face::Finder(*configuration);										  },
		  	[&](){ eyes_finder_        = new FRsdk::Eyes::Finder(*configuration);										  },
			  [&](){ portrait_analyzer_  = new FRsdk::Portrait::Analyzer(*configuration);								},
			  [&](){ iso_19794_test_     = new FRsdk::ISO_19794_5::FullFrontal::Test(*configuration);		},
			  [&](){ feature_test_       = new FRsdk::Portrait::Feature::Test(*configuration);					},
			  [&](){ token_face_creator_ = new FRsdk::ISO_19794_5::TokenFace::Creator(*configuration);	}
			);
			
			return true;
		}
		catch (const FRsdk::FeatureDisabled& e) { std::cout << e.what(); return false; }
		catch (const FRsdk::LicenseSignatureMismatch& e) { std::cout << e.what(); return false; }
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}
	
	void FacialAcquisition::find_face(FaceVacsImage image, std::vector<FRsdkEntities::FaceVacsFullFace>& faces)
	{
		try
		{			
			auto faceLocations =
				face_finder_->find(*image, MIN_EYE_DISTANCE, MAX_EYE_DISTANCE);

			if (faceLocations.size() < 1)
			{
				std::cout << "any face not found" << std::endl;
				return;
			}

		
			parallel_for_each(faceLocations.cbegin(), faceLocations.cend(),
				[&](FRsdk::Face::Location face)
			{
				auto eyesLocations = eyes_finder_->find(*image, face);
				if (eyesLocations.size() > 0)
				{
					FRsdkEntities::FaceVacsFullFace fullface(face, eyesLocations.front());
					faces.push_back(fullface);			
				}
			});
			
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
	
	FaceVacsPortraitCharacteristicsPtr FacialAcquisition::analyze(const FRsdk::AnnotatedImage& image) const
	{
		try	{
			FaceVacsPortraitCharacteristicsPtr pch(
				new FRsdk::Portrait::Characteristics(portrait_analyzer_->analyze(image)));
			return pch;
		}
		catch (std::exception& ex)	{
			std::cout << ex.what() << std::endl;
		}
		return nullptr;
	}

	FaceVacsCompliancePtr FacialAcquisition::iso_compliance_test(const FRsdk::Portrait::Characteristics& pch) const
	{
		try	{
			FaceVacsCompliancePtr result(new FRsdk::ISO_19794_5::FullFrontal::Compliance
				(iso_19794_test_->assess(pch)));

			return result;
		}
		catch (std::exception& ex)	{
			std::cout << ex.what() << std::endl;
		}
		return nullptr;
	}

	FaceVacsImage FacialAcquisition::extract_face(const FRsdk::AnnotatedImage& image) const
	{
		try	{
			FaceVacsImage result(new FRsdk::Image(token_face_creator_->extract(image).first));
			return result;
		}
		catch (std::exception& ex)	{
			std::cout << ex.what() << std::endl;
		}
		return nullptr;
	}


}

#include "biotasks/facial_acquisition.hpp"

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
			std::cout << e.what();		}

	}

	FacialAcquisition::FacialAcquisition(FRsdkTypes::FaceVacsConfiguration configuration)
	{
		init(configuration);			
	}

	bool FacialAcquisition::init(FRsdkTypes::FaceVacsConfiguration configuration)
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


}
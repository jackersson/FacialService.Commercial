#include "biotasks/facial_acquisition.hpp"
#include "utils/face_vacs_io_utils.hpp"

#include <iostream>

#include <Windows.h>
#include <ppl.h>

namespace BioFacialEngine
{
	FacialAcquisition::FacialAcquisition(const std::string& configuration_filename)
	{
		bool is_ok = false;
		try
		{
			FaceVacsConfiguration configuration = new FRsdk::Configuration(configuration_filename);
			is_ok = init(configuration);
	  }
	  catch (const FRsdk::FeatureDisabled& e) { 
			std::cout << e.what();
		}
	  catch (const FRsdk::LicenseSignatureMismatch& e) {
			std::cout << e.what();
		}	 
	  catch (std::exception& e) {
			std::cout << e.what();
		}

		if (!is_ok)
			this->~FacialAcquisition();
	}

	FacialAcquisition::FacialAcquisition(FaceVacsConfiguration configuration)
	{
		if (!init(configuration))
			FacialAcquisition::~FacialAcquisition();
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


}
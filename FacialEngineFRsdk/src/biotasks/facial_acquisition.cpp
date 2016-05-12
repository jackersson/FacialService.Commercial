#include "biotasks/facial_acquisition.hpp"
#include "utils/face_vacs_io_utils.hpp"

#include <iostream>
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
			face_finder_        = new FRsdk::Face::Finder(*configuration);
			eyes_finder_        = new FRsdk::Eyes::Finder(*configuration);
			portrait_analyzer_  = new FRsdk::Portrait::Analyzer(*configuration);
			iso_19794_test_     = new FRsdk::ISO_19794_5::FullFrontal::Test(*configuration);
			feature_test_       = new FRsdk::Portrait::Feature::Test(*configuration);
			token_face_creator_ = new FRsdk::ISO_19794_5::TokenFace::Creator(*configuration);
			return true;
		}
		catch (const FRsdk::FeatureDisabled& e) { std::cout << e.what(); return false; }
		catch (const FRsdk::LicenseSignatureMismatch& e) { std::cout << e.what(); return false; }
		catch (std::exception& e) { std::cout << e.what(); return false; }

	}

	ImageCharacteristicsType
	FacialAcquisition::acquire(FaceVacsImage image)
	{		
		FRsdk::Face::LocationSet faceLocations =
			face_finder_->find(*image, MIN_EYE_DISTANCE, MAX_EYE_DISTANCE);		

		if (faceLocations.size() < 1)		
			throw AcquisitionError("Unable to locate face");

		std::shared_ptr<ImageCharacteristics> image_characteristics(new ImageCharacteristics());

		for (auto face = faceLocations.begin(); face != faceLocations.end(); ++face)
		{
			FRsdk::Eyes::LocationSet eyesLocations = 	eyes_finder_->find(*image, *face);
			image_characteristics->push(*face, eyesLocations);			
		}

		image_characteristics->analyze(*image, *portrait_analyzer_, *iso_19794_test_, *token_face_creator_);		

		return image_characteristics;
	}

	ImageCharacteristicsType
	FacialAcquisition::acquire(const std::string& filename)
	{
		//FaceVacsImage image = new FRsdk::Image(FRsdk::Jpeg::load(filename));
		try
		{
			FaceVacsIOUtils face_vacs_io_utils;
			FaceVacsImage image = face_vacs_io_utils.loadFromFile(filename);
			ImageCharacteristicsType image_characteristics = acquire(image);

			return image_characteristics;
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}

		return NULL;
	}
}
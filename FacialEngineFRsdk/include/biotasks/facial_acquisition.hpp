#ifndef FacialAcquisition_INCLUDED
#define FacialAcquisition_INCLUDED

#include "engine/ifacial_engine.hpp"
#include "feedback/facial_acquisiton_feedback.hpp"

#include "utils/frsdk_face_characteristics.hpp"

#include <frsdk/config.h>
#include <frsdk/image.h>
#include <frsdk/tokenface.h>
#include <frsdk/portrait.h>
#include <frsdk/eyes.h>
#include <frsdk/cbeff.h>

#include <frsdk/jpeg.h>
#include <frsdk/j2k.h>
#include <frsdk/bmp.h>
#include <frsdk/png.h>
#include <frsdk/pgm.h>

#include <frsdk/portraittests.h>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Image>                           FaceVacsImage;
	typedef FRsdk::CountedPtr<FRsdk::Face::Finder>                    FaceVacsFaceFinder;
	typedef FRsdk::CountedPtr<FRsdk::Eyes::Finder>                    FaceVacsEyesFinder;
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Analyzer>              FaceVacsPortraitAnalyzer;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Test>  FaceVacsIso19794Test;
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Feature::Test>         FaceVacsFeatureTest;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::TokenFace::Creator> FaceVacsTfcreator;
	typedef FRsdk::CountedPtr<FRsdk::Configuration>                   FaceVacsConfiguration;

	typedef std::shared_ptr<ImageCharacteristics> ImageCharacteristicsType;

	class FacialAcquisition 
	{

	public:
		FacialAcquisition(const std::string& configuration_filename);
		FacialAcquisition(FaceVacsConfiguration configuration);
		~FacialAcquisition() {}

		ImageCharacteristicsType acquire(const std::string& filename);

		ImageCharacteristicsType acquire(FaceVacsImage image);

	private:
		bool init(FaceVacsConfiguration configuration);

	private:
		FaceVacsFaceFinder        face_finder_;
		FaceVacsEyesFinder        eyes_finder_;
		FaceVacsPortraitAnalyzer  portrait_analyzer_;
		FaceVacsIso19794Test      iso_19794_test_;
		FaceVacsFeatureTest       feature_test_;
		FaceVacsTfcreator         token_face_creator_;

	private:
		const float MIN_EYE_DISTANCE = 0.05f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};

	
	class AcquisitionError : public std::exception
	{
	public:
		AcquisitionError(const std::string& msg_) throw() : msg(msg_) {}
		~AcquisitionError() throw() { }
		const char* what() const throw() { return msg.c_str(); }
	private:
		std::string msg;
	};
	
}

#endif
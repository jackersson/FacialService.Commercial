#ifndef FacialAcquisition_INCLUDED
#define FacialAcquisition_INCLUDED

#include "utils/frsdk_face_characteristics.hpp"
#include "utils/face_vacs_io_utils.hpp"

namespace BioFacialEngine
{	
	typedef FRsdk::CountedPtr<FRsdk::Face::Finder>                    FaceVacsFaceFinder      ;
	typedef FRsdk::CountedPtr<FRsdk::Eyes::Finder>                    FaceVacsEyesFinder      ;
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Analyzer>              FaceVacsPortraitAnalyzer;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Test>  FaceVacsIso19794Test    ;
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Feature::Test>         FaceVacsFeatureTest     ;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::TokenFace::Creator> FaceVacsTfcreator       ;
	typedef FRsdk::CountedPtr<FRsdk::Configuration>                   FaceVacsConfiguration   ;

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
		FaceVacsFaceFinder        face_finder_       ;
		FaceVacsEyesFinder        eyes_finder_       ;
		FaceVacsPortraitAnalyzer  portrait_analyzer_ ;
		FaceVacsIso19794Test      iso_19794_test_    ;
		FaceVacsFeatureTest       feature_test_      ;
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
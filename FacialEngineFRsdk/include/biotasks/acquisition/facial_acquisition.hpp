#ifndef FacialAcquisition_INCLUDED
#define FacialAcquisition_INCLUDED

#include "utils/face_vacs_includes.hpp"
#include "wrappers/face_info.hpp"

#include <frsdk/tokenface.h>

namespace BioFacialEngine
{	
	typedef FRsdk::CountedPtr<FRsdk::Face::Finder>                    FaceVacsFaceFinder      ;
	typedef FRsdk::CountedPtr<FRsdk::Eyes::Finder>                    FaceVacsEyesFinder      ;
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Analyzer>              FaceVacsPortraitAnalyzer;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Test>  FaceVacsIso19794Test    ;
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Feature::Test>         FaceVacsFeatureTest     ;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::TokenFace::Creator> FaceVacsTfcreator       ;	
	
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Characteristics>             FaceVacsPortraitCharacteristicsPtr;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Compliance>  FaceVacsCompliancePtr;	

	class FacialAcquisition 
	{
	public:
		explicit FacialAcquisition(const std::string& configuration_filename);
		explicit FacialAcquisition(FRsdkTypes::FaceVacsConfiguration configuration);
		~FacialAcquisition() {}

		void find_face(FRsdkTypes::FaceVacsImage image, std::vector<FRsdkEntities::FaceVacsFullFace>& faces);
				

		FaceVacsPortraitCharacteristicsPtr analyze            (const FRsdk::AnnotatedImage& image) const;
		FaceVacsCompliancePtr              iso_compliance_test(const FRsdk::Portrait::Characteristics& pch) const;
		FRsdkTypes::FaceVacsImage          extract_face       (const FRsdk::AnnotatedImage& image) const;
	

	private:
		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

		FaceVacsFaceFinder        face_finder_       ;
		FaceVacsEyesFinder        eyes_finder_       ;
		FaceVacsPortraitAnalyzer  portrait_analyzer_ ;
		FaceVacsIso19794Test      iso_19794_test_    ;
		FaceVacsFeatureTest       feature_test_      ;
		FaceVacsTfcreator         token_face_creator_;

		const float MIN_EYE_DISTANCE = 0.05f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};

	typedef std::shared_ptr<FacialAcquisition> FaceVacsAcquisitionPtr;
	
	class AcquisitionError : public std::exception
	{
	public:
		explicit AcquisitionError(const std::string& msg_) throw() : msg(msg_) {}
		~AcquisitionError() throw() { }
		const char* what() const  throw() { return msg.c_str(); }
	private:
		std::string msg;
	};
	
}

#endif
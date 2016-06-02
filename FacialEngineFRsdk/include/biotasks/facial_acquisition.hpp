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

		void findFace(FRsdkTypes::FaceVacsImage image, std::vector<FRsdkEntities::FaceVacsFullFace>& faces)
		{
			try
			{
				auto faceLocations =
					face_finder_->find(*image, MIN_EYE_DISTANCE, MAX_EYE_DISTANCE);

				if (faceLocations.size() < 1)
				{
					std::cout << "any face not found";
					return;
				}

				concurrency::parallel_for_each(faceLocations.cbegin(), faceLocations.cend(),
					[&](FRsdk::Face::Location face)
				{
					auto eyesLocations = eyes_finder_->find(*image, face);
					if (eyesLocations.size() > 0)
						faces.push_back(FRsdkEntities::FaceVacsFullFace(face, eyesLocations.front()));
				});
			}
			catch (std::exception& ex)
			{
				std::cout << ex.what() << std::endl;
			}
		}

		FaceVacsPortraitCharacteristicsPtr analyze(const FRsdk::AnnotatedImage& image) const
		{			
			try	{			
				FaceVacsPortraitCharacteristicsPtr pch (
					new FRsdk::Portrait::Characteristics(portrait_analyzer_->analyze(image)));
				return pch;
			}
			catch (std::exception& ex)	{			
				std::cout << ex.what() << std::endl;
			}
			return nullptr;
		}

		FaceVacsCompliancePtr isoComplianceTest(const FRsdk::Portrait::Characteristics& pch) const
		{		
			try	{
				FaceVacsCompliancePtr result ( new FRsdk::ISO_19794_5::FullFrontal::Compliance 
					                                ( iso_19794_test_->assess(pch)));

				return result;
			}
			catch (std::exception& ex)	{				
				std::cout << ex.what() << std::endl;
			}
			return nullptr;
		}

		FRsdkTypes::FaceVacsImage extractFace(const FRsdk::AnnotatedImage& image) const
		{			
			try	{
				FRsdkTypes::FaceVacsImage result( new FRsdk::Image(token_face_creator_->extract(image).first) );
				return result;
			}
			catch (std::exception& ex)	{			
				std::cout << ex.what() << std::endl;
			}
			return nullptr;
		}

	private:
		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

	private:
		FaceVacsFaceFinder        face_finder_       ;
		FaceVacsEyesFinder        eyes_finder_       ;
		FaceVacsPortraitAnalyzer  portrait_analyzer_ ;
		FaceVacsIso19794Test      iso_19794_test_    ;
		FaceVacsFeatureTest       feature_test_      ;
		FaceVacsTfcreator         token_face_creator_;

	private:
		const float MIN_EYE_DISTANCE = 0.1f;
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
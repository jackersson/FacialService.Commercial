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

	typedef std::pair<FRsdk::Face::Location, FRsdk::Eyes::Location>    FaceVacsFullFace;
	
	typedef FRsdk::CountedPtr<FRsdk::Portrait::Characteristics>             FaceVacsPortraitCharacteristicsPtr;
	typedef FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Compliance>  FaceVacsCompliancePtr;	

	class FacialAcquisition 
	{

	public:
		FacialAcquisition(const std::string& configuration_filename);
		FacialAcquisition(FaceVacsConfiguration configuration);
		~FacialAcquisition() {}

		void findFace(FaceVacsImage image, std::vector<FaceVacsFullFace>& faces)
		{
			try
			{
				FRsdk::Face::LocationSet faceLocations =
					face_finder_->find(*image, MIN_EYE_DISTANCE, MAX_EYE_DISTANCE);

				if (faceLocations.size() < 1)
				{
					std::cout << "any face not found";
					return;
				}

				concurrency::parallel_for_each(faceLocations.cbegin(), faceLocations.cend(),
					[&](FRsdk::Face::Location face)
				{
					FRsdk::Eyes::LocationSet eyesLocations = eyes_finder_->find(*image, face);
					if (eyesLocations.size() > 0)
						faces.push_back(FaceVacsFullFace(face, eyesLocations.front()));
				});
			}
			catch (std::exception& ex)
			{
				std::cout << ex.what() << std::endl;
			}
		}

		FaceVacsPortraitCharacteristicsPtr analyze(const FRsdk::AnnotatedImage& image)
		{			
			try	{			
				FaceVacsPortraitCharacteristicsPtr pch 
					= new FRsdk::Portrait::Characteristics(portrait_analyzer_->analyze(image));
				return pch;
			}
			catch (std::exception& ex)	{			
				std::cout << ex.what() << std::endl;
			}
			return nullptr;
		}

		FaceVacsCompliancePtr isoComplianceTest(const FRsdk::Portrait::Characteristics& pch)
		{		
			try	{
				FaceVacsCompliancePtr result = new FRsdk::ISO_19794_5::FullFrontal::Compliance 
					                                ( iso_19794_test_->assess(pch));

				return result;
			}
			catch (std::exception& ex)	{				
				std::cout << ex.what() << std::endl;
			}
			return nullptr;
		}

		FaceVacsImage extractFace(const FRsdk::AnnotatedImage& image)
		{			
			try	{
				FaceVacsImage result = new FRsdk::Image(token_face_creator_->extract(image).first);
				return result;
			}
			catch (std::exception& ex)	{			
				std::cout << ex.what() << std::endl;
			}
			return nullptr;
		}

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
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};

	typedef std::shared_ptr<FacialAcquisition> FaceVacsAcquisitionPtr;
	
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
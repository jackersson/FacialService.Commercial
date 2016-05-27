#ifndef FacialEngine_INCLUDED
#define FacialEngine_INCLUDED

#include "engine/ifacial_engine.hpp"

#include "biotasks/facial_acquisition.hpp"
#include "biotasks/facial_enrollment.hpp"
#include "biotasks/facial_verification.hpp"
#include "wrappers/image_info.hpp"

#include <string>

namespace BioFacialEngine
{

	class FacialEngine : public BioContracts::IFacialEngine
	{
	public:
		explicit FacialEngine(const std::string& frsdk_configuration);
		virtual ~FacialEngine() {}

		/*
		BioContracts::IImageInfoPtr
		acquire(const std::string& filename) override
		{
			std::string shwarc = "C:\\Users\\jacke\\Desktop\\3423.jpg";
			FRsdkEntities::ImageInfoPtr ptr(new FRsdkEntities::ImageInfo(shwarc));
			return ptr;
		} */

		BioContracts::IImageInfoPtr
			acquire(const BioContracts::RawImage& raw_image) override
		{
			std::string shwarc = "C:\\Users\\jacke\\Desktop\\3423.jpg";
			FRsdkEntities::ImageInfoPtr ptr(new FRsdkEntities::ImageInfo(shwarc));
			return ptr;
		}
		
		/*
		BioContracts::IImageInfoPtr
			enroll(const std::string& filename) override
		{
			std::string shwarc = "C:\\Users\\jacke\\Desktop\\3423.jpg";
			FRsdkEntities::ImageInfoPtr ptr(new FRsdkEntities::ImageInfo(shwarc));
			return ptr;
		}
		

		BioContracts::IImageInfoPtr
			enroll(const BioContracts::RawImage& raw_image) override
		{
			std::string shwarc = "C:\\Users\\jacke\\Desktop\\3423.jpg";
			FRsdkEntities::ImageInfoPtr ptr(new FRsdkEntities::ImageInfo(shwarc));
			return ptr;
		}
		*/

	private:
		FacialEngine(const FacialEngine&);

		std::unique_ptr<FacialAcquisition>  acquisition_ ;
		std::unique_ptr<FacialEnrollmentProcessor>   enrollment_  ;
		std::unique_ptr<FacialVerification> verification_;



	};

	typedef std::shared_ptr<BioFacialEngine::FacialEngine> FacialEnginePtr;
}

#endif
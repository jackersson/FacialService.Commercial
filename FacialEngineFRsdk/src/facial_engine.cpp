
#include "facial_engine.hpp"
#include "utils/face_vacs_io_utils.hpp"


namespace BioFacialEngine
{

	FacialEngine::FacialEngine( const std::string& frsdk_configuration)
		                        : acquisition_ ( new FacialAcquisition (frsdk_configuration))
		                        , enrollment_  ( new FacialEnrollment  (frsdk_configuration))
														, verification_( new FacialVerification(frsdk_configuration))
	{
	}

	
	ImageCharacteristicsType
		FacialEngine::enrollFromFile(const std::string& filename)
	{
		FaceVacsIOUtils io_utils;
		FaceVacsImage image = io_utils.loadFromFile(filename);
	
		return enrollPerformer(image);
	}

	BioContracts::ImageCharacteristicsConstRef
		FacialEngine::enroll(const std::string& filename)
	{
		return enrollFromFile(filename);
	}

	BioContracts::ImageCharacteristicsConstRef
	FacialEngine::enroll(const BioContracts::RawImage& raw_image)
	{
		return enrollPerformer(raw_image);
	}

	ImageCharacteristicsType
	FacialEngine::enrollPerformer(FaceVacsImage image)
	{		
		ImageCharacteristicsType im_ch(acquisition_->acquire(image));

		if (im_ch->hasFaces())
		{
			FRsdkFaceCharacteristic face = im_ch->getEnrollmentAbleFace();
			enrollment_->enroll(face);
		}
		return im_ch;
	}

	ImageCharacteristicsType
	FacialEngine::enrollPerformer(const BioContracts::RawImage& raw_image)
	{
		FaceVacsIOUtils io_utils;
		FaceVacsImage image = io_utils.loadFromBytes(raw_image.bytes(), raw_image.size());

		ImageCharacteristicsType im_ch(acquisition_->acquire(image));

		if (im_ch->hasFaces())
		{
			FRsdkFaceCharacteristic face = im_ch->getEnrollmentAbleFace();
			enrollment_->enroll(face);
		}
		return im_ch;
	}

	
	BioContracts::VerificationResult
		FacialEngine::verify( const BioContracts::RawImage& target_raw_image
		                    , const BioContracts::RawImage& comparison_raw_image)
	{		
		ImageCharacteristicsType target     = enrollPerformer(target_raw_image    );
		ImageCharacteristicsType comparison = enrollPerformer(comparison_raw_image);
		BioContracts::Matches matches;
//		BioContracts::Matches matches = verification_->verify(target, comparison);
		BioContracts::VerificationResult vr(matches, target, comparison);
		return vr;
	}

	BioContracts::VerificationResult
		FacialEngine::verify( const std::string& first
		                    , const std::string& second)
	{
		ImageCharacteristicsType target     = enrollFromFile(first);
		ImageCharacteristicsType comparison = enrollFromFile(second);

		BioContracts::Matches matches;
		//		BioContracts::Matches matches = verification_->verify(target, comparison);
		BioContracts::VerificationResult vr(matches, target, comparison);
		return vr;		
	}

	/*
	void FacialEngine::identify(const BioService::VerificationData& images){

	}
	*/

	BioContracts::ImageCharacteristicsConstRef
		FacialEngine::acquire(const std::string& filename)
	{
		FaceVacsIOUtils io_utils;
		FaceVacsImage image = io_utils.loadFromFile(filename);

		//	try
		//		{
		ImageCharacteristicsType im(acquisition_->acquire(image));
		return im;
		//	}
		//	catch (std::exception& ex)
		//	{
		//	std::cout << ex.what() << std::endl;
		//}

		//return;
	}


	BioContracts::ImageCharacteristicsConstRef
		FacialEngine::acquire(const BioContracts::RawImage& raw_image)
	{
		FaceVacsIOUtils io_utils;
		FaceVacsImage image = io_utils.loadFromBytes(raw_image.bytes(), raw_image.size());

	//	try
//		{
			ImageCharacteristicsType im(acquisition_->acquire(image));
			return im;
	//	}
	//	catch (std::exception& ex)
	//	{
		//	std::cout << ex.what() << std::endl;
		//}

		//return;
	}

}

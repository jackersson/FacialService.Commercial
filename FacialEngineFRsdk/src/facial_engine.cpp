
#include "facial_engine.hpp"
#include "utils/face_vacs_io_utils.hpp"


namespace BioFacialEngine
{

	FacialEngine::FacialEngine( const std::string& frsdk_configuration)
		                        : acquisition_(new FacialAcquisition(frsdk_configuration))
		                       // , enrollment_ (new FacialEnrollment (frsdk_configuration))
	{
	}

	
	BioContracts::ImageCharacteristicsConstRef
	FacialEngine::enroll(const std::string& filename ) 
	{
		FaceVacsIOUtils io_utils;

		FaceVacsImage image = io_utils.loadFromFile(filename);
				
	 	ImageCharacteristicsType im_ch(acquisition_->acquire(image));		

		if (im_ch->hasFaces())
		{
			IEnrollmentAble& fc = im_ch->getEnrollmentAbleFace();
			enrollment_->enroll(fc);
		}
		return im_ch;
	}

	BioContracts::ImageCharacteristicsConstRef
		FacialEngine::enroll(const std::string& image_bytestring, size_t size)
	{
		FaceVacsIOUtils io_utils;

		FaceVacsImage image = io_utils.loadFromBytes(image_bytestring, image_bytestring.size());

		ImageCharacteristicsType im_ch(acquisition_->acquire(image));

		if (im_ch->hasFaces())
		{
			IEnrollmentAble& fc = im_ch->getEnrollmentAbleFace();
			enrollment_->enroll(fc);
		}
		return im_ch;

	}

	/*
	void FacialEngine::verify(const BioService::VerificationData& verification_data){

	}

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
	FacialEngine::acquire(const std::string& image_bytestring, size_t size)
	{
		FaceVacsIOUtils io_utils;
		FaceVacsImage image = io_utils.loadFromBytes(image_bytestring, image_bytestring.size());

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

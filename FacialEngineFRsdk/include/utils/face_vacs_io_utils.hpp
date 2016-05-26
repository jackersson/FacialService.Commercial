#ifndef FaceVacsIOUtils_INCLUDED
#define FaceVacsIOUtils_INCLUDED

#include "utils/face_vacs_includes.hpp"
#include "protobufs/bio_service.pb.h"


namespace BioFacialEngine
{
	class FaceVacsIOUtils
	{
	public:
		FRsdkTypes::FaceVacsImage loadFromFile(const std::string& filename);	

		FRsdkTypes::FaceVacsImage loadFromBytes(const std::string& image_bytestring
			                         , const size_t size
			                         , BioService::ImageFormat image_format = BioService::ImageFormat::JPEG);
		
	private:
	
		FRsdkTypes::FaceVacsImage loadJpeg(const std::string& filename);

		FRsdkTypes::FaceVacsImage loadJpeg(const std::string& image_bytestring, const size_t size);

		//FaceVacsImage loadJpeg2000(const std::string& filename){}
		//FaceVacsImage loadBmp     (const std::string& filename){}
		//FaceVacsImage loadPgm     (const std::string& filename){}
		//FaceVacsImage loadPng     (const std::string& filename){}

		BioService::ImageFormat getFormat(const std::string& format);		

	private:
		static std::map<std::string, BioService::ImageFormat> image_formats_;
	};
}

#endif
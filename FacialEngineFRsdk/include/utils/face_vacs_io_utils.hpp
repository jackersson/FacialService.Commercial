#ifndef FaceVacsIOUtils_INCLUDED
#define FaceVacsIOUtils_INCLUDED

#include "utils/face_vacs_includes.hpp"
#include "protobufs/bio_service.pb.h"


namespace BioFacialEngine
{
	class FaceVacsIOUtils
	{
	public:
		FRsdkTypes::FaceVacsImage loadFromFile(const std::string& filename) const;


		FRsdkTypes::FaceVacsImage loadFromBytes( const std::string& image_bytestring	
		                                       , BioService::ImageFormat image_format = BioService::ImageFormat::JPEG) const;
		
		BioService::ImageFormat getFormat(const std::string& format) const;

	private:
	
		FRsdkTypes::FaceVacsImage loadJpeg(const std::string& filename) const;
		FRsdkTypes::FaceVacsImage loadJpeg(const std::string& image_bytestring, const size_t size) const;	

		FRsdkTypes::FaceVacsImage loadBmp(const std::string& filename) const;
		FRsdkTypes::FaceVacsImage loadBmp(const std::string& image_bytestring, const size_t size) const;
		FRsdkTypes::FaceVacsImage loadPng(const std::string& filename) const;
		FRsdkTypes::FaceVacsImage loadPng(const std::string& image_bytestring, const size_t size) const;



	private:
		static std::map<std::string, BioService::ImageFormat> image_formats_;
	};
}

#endif
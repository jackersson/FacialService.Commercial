#ifndef FaceVacsIOUtils_INCLUDED
#define FaceVacsIOUtils_INCLUDED

#include "engine/ifacial_engine.hpp"

#include "biotasks/facial_acquisition.hpp"
#include "protobufs/bio_service.pb.h"

namespace BioFacialEngine
{
	class FaceVacsIOUtils
	{
	public:
		FaceVacsImage loadFromFile(const std::string& filename)
		{
			if (filename.size() < 1)
				return NULL;

			char period( '.');
			std::string format ("");
			for (auto ch = filename.rbegin(); ch != filename.rend(); ++ch)
			{				
				if (*ch == period)
					break;

				format += *ch;
			}

			std::reverse(format.begin(), format.end());
		
			try
			{
				BioService::ImageFormat image_format = getFormat(format);
				switch (image_format)
				{
				case BioService::JPEG:
					return loadJpeg(filename);					
				case BioService::JPEG2000:
					break;
				case BioService::PNG:
					break;
				case BioService::BMP:
					break;
				case BioService::PGM:
					break;
				case BioService::GRAYSCALE_8BIT:
					break;		
				default:
					break;
				}
			}
			catch (std::invalid_argument& ex)
			{
				std::cout << ex.what() << std::endl;
			}

			throw std::invalid_argument("Image format is not supported");
		}

		FaceVacsImage loadFromBytes( const std::string& image_bytestring
			                         , const size_t size
															 , BioService::ImageFormat image_format = BioService::ImageFormat::JPEG )
		{
			switch (image_format)
			{
			case BioService::ImageFormat::JPEG:
				return loadJpeg(image_bytestring, size);
			}

			throw std::invalid_argument("Image format is not supported");
		}
			
	private:
	
		FaceVacsImage loadJpeg  (const std::string& filename)
		{
			try
			{
				FaceVacsImage img (new FRsdk::Image( FRsdk::Jpeg::load(std::string(filename))));
				return img;
			}
			catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}

			return NULL;
		}

		FaceVacsImage loadJpeg(const std::string& image_bytestring, const size_t size )
		{
			try
			{
				FaceVacsImage img(new FRsdk::Image(FRsdk::Jpeg::load( image_bytestring.c_str(), size)));
				return img;
			}
			catch (std::exception& e)	{
				std::cout << e.what() << std::endl;
			}

			return NULL;
		}	

		//FaceVacsImage loadJpeg2000(const std::string& filename){}
		//FaceVacsImage loadBmp     (const std::string& filename){}
		//FaceVacsImage loadPgm     (const std::string& filename){}
		//FaceVacsImage loadPng     (const std::string& filename){}

		BioService::ImageFormat getFormat(const std::string& format)
		{			
			auto it = image_formats_.find(format);
			if (it != image_formats_.end())
				return (*it).second;
			else
				throw std::invalid_argument("Image format is not supported");
		}	

	private:
		static std::map<std::string, BioService::ImageFormat> image_formats_;



	};
}

#endif
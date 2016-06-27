#include "utils/face_vacs_io_utils.hpp"

#include <frsdk/jpeg.h>
#include <frsdk/j2k.h>
#include <frsdk/bmp.h>
#include <frsdk/png.h>
#include <frsdk/pgm.h>

#include <sstream>

using namespace FRsdkTypes;

namespace BioFacialEngine
{
	
	std::map<std::string, BioService::ImageFormat> FaceVacsIOUtils::image_formats_ = {
		    { "jpg", BioService::ImageFormat::JPEG }
	    , { "png", BioService::ImageFormat::PNG  }
		  , { "bmp", BioService::ImageFormat::BMP  }
		};

	std::string get_file_format(const std::string& filename)
	{
		char period('.');
		std::string format("");
		for (auto ch = filename.rbegin(); ch != filename.rend(); ++ch)
		{
			if (*ch == period)
				break;

			format += *ch;
		}

		std::reverse(format.begin(), format.end());
		return format;
	}
	
  FaceVacsImage FaceVacsIOUtils::loadFromFile(const std::string& filename)
  {	
  	if (filename.size() < 1)
  		throw std::exception("Filename is not valid");  
  	
  	std::string format(get_file_format(filename));  
  	auto image_format = getFormat(format);
  	switch (image_format)
  	{
  	case BioService::JPEG:
  		return loadJpeg(filename);  
  	case BioService::PNG:
			return loadPng(filename);
  	case BioService::BMP:
			return loadJpeg(filename);
  	case BioService::PGM:  	
		case BioService::JPEG2000:
  	case BioService::GRAYSCALE_8BIT:
  	default:
			throw std::invalid_argument("Image format is not supported");
  	}   
  }

  FaceVacsImage FaceVacsIOUtils::loadFromBytes( const std::string& image_bytestring			                                       
			                                        , BioService::ImageFormat image_format )
  {
  	switch (image_format)
  	{
  	case BioService::JPEG:
  		return loadJpeg(image_bytestring, image_bytestring.size());
		case BioService::PNG:
			return loadPng(image_bytestring, image_bytestring.size());
		case BioService::BMP:
			return loadBmp(image_bytestring, image_bytestring.size());

	  case BioService::JPEG2000 :
	  case BioService::PGM      :
	  case BioService::GRAYSCALE_8BIT:

	  case BioService::ImageFormat_INT_MIN_SENTINEL_DO_NOT_USE_:
	  case BioService::ImageFormat_INT_MAX_SENTINEL_DO_NOT_USE_:
	  default: 
			throw std::invalid_argument("Image format is not supported");
	  }   	
  }


	FaceVacsImage FaceVacsIOUtils::loadJpeg(const std::string& filename) const
  {  	
  	FaceVacsImage img(new FRsdk::Image(FRsdk::Jpeg::load(std::string(filename))));
  	return img;  
  }

	FaceVacsImage FaceVacsIOUtils::loadJpeg(const std::string& image_bytestring, const size_t size)
  {
  	try
  	{
  		FaceVacsImage img(new FRsdk::Image(FRsdk::Jpeg::load(image_bytestring.c_str(), size)));
  		return img;
  	}
  	catch (std::exception& e)	{
  		std::cout << e.what() << std::endl;
  	}
  
  	return NULL;
  }

	FaceVacsImage FaceVacsIOUtils::loadPng(const std::string& filename)
	{
		try
		{
			FaceVacsImage img(new FRsdk::Image(FRsdk::Png::load(std::string(filename))));
			return img;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return NULL;
	}

	FaceVacsImage FaceVacsIOUtils::loadPng(const std::string& image_bytestring, const size_t size)
	{
		try
		{
			std::stringstream png_stream;
			png_stream << image_bytestring;
			FaceVacsImage img(new FRsdk::Image(FRsdk::Png::load(png_stream)));
			return img;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return NULL;
	}

	FaceVacsImage FaceVacsIOUtils::loadBmp(const std::string& filename)
	{
		try
		{
			FaceVacsImage img(new FRsdk::Image(FRsdk::Bmp::load(std::string(filename))));
			return img;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return NULL;
	}

	FaceVacsImage FaceVacsIOUtils::loadBmp(const std::string& image_bytestring, const size_t size)
	{
		try
		{
			std::stringstream png_stream;
			png_stream << image_bytestring;
			FaceVacsImage img(new FRsdk::Image(FRsdk::Bmp::load(png_stream)));
			return img;
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return NULL;
	}

		//FaceVacsImage loadJpeg2000(const std::string& filename){}
		//FaceVacsImage loadBmp     (const std::string& filename){}
		//FaceVacsImage loadPgm     (const std::string& filename){}
		//FaceVacsImage loadPng     (const std::string& filename){}

	BioService::ImageFormat FaceVacsIOUtils::getFormat(const std::string& format)
	{
		auto it = image_formats_.find(format);
		if (it != image_formats_.end())
			return (*it).second;
		else
			throw std::invalid_argument("Image format is not supported");
	}
	


}

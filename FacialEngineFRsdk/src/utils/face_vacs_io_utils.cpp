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

	
  FaceVacsImage FaceVacsIOUtils::loadFromFile(const std::string& filename)
  {
		std::cout << "loading " << filename << std::endl;

  	if (filename.size() < 1)
  		return nullptr;
  
  	char period('.');
  	std::string format("");
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

  FaceVacsImage FaceVacsIOUtils::loadFromBytes( const std::string& image_bytestring
			                                        , const size_t size
			                                        , BioService::ImageFormat image_format )
  {
  	switch (image_format)
  	{
  	case BioService::ImageFormat::JPEG:
  		return loadJpeg(image_bytestring, size);
		case BioService::ImageFormat::PNG:
			return loadPng(image_bytestring, size);	
		case BioService::ImageFormat::BMP:
			return loadPng(image_bytestring, size);
	
  	}
  
  	throw std::invalid_argument("Image format is not supported");
  }


	FaceVacsImage FaceVacsIOUtils::loadJpeg(const std::string& filename)
  {
  	try
  	{
  		FaceVacsImage img(new FRsdk::Image(FRsdk::Jpeg::load(std::string(filename))));
  		return img;
  	}
  	catch (std::exception& e) {
  		std::cout << e.what() << std::endl;
  	}
  
  	return NULL;
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

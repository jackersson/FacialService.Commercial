#include "utils/face_vacs_io_utils.hpp"

namespace BioFacialEngine
{
	std::map<std::string, BioService::ImageFormat> FaceVacsIOUtils::image_formats_ = {
		  { "jpg", BioService::ImageFormat::JPEG }
		};
}

#ifndef FaceVacs_INCLUDED
#define FaceVacs_INCLUDED

#include <frsdk/config.h>
#include <frsdk/image.h>
#include <memory>

namespace FRsdkTypes
{
	typedef std::shared_ptr<FRsdk::Configuration>                   FaceVacsConfiguration;
	typedef FRsdk::CountedPtr<FRsdk::Image>                           FaceVacsImage        ;	
}
#endif
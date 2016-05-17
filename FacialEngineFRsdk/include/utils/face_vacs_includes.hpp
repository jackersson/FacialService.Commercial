#ifndef FaceVacs_INCLUDED
#define FaceVacs_INCLUDED

#include <frsdk/config.h>
#include <frsdk/image.h>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Configuration>                   FaceVacsConfiguration;
	typedef FRsdk::CountedPtr<FRsdk::Image>                           FaceVacsImage        ;	
}
#endif
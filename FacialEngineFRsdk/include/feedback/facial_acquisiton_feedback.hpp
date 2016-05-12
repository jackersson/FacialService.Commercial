#ifndef FacialAcquisitionFeedback_INCLUDED
#define FacialAcquisitionFeedback_INCLUDED

#include "engine/ifacial_engine.hpp"

#include <frsdk/config.h>
#include <frsdk/image.h>
#include <frsdk/tokenface.h>
#include <frsdk/portrait.h>
#include <frsdk/eyes.h>
#include <frsdk/cbeff.h>

namespace BioFacialEngine
{
	class FacialAcquisitionFeedback
	{
	public:
		FacialAcquisitionFeedback();
		~FacialAcquisitionFeedback();

		void onFaceFound(const FRsdk::Face::Location& face, const FRsdk::Eyes::LocationSet& eyes)
		{		
		}

		void onFaceNotFound()
		{
		}

		void onFaceFound(const FRsdk::Face::LocationSet& locations )
		{	
		}




	private:

		
		BioService::PortraitCharacteristic pc;
	};
}

#endif
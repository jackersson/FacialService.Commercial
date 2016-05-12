#ifndef IBioDatabaseClient_INCLUDED
#define IBioDatabaseClient_INCLUDED

#include "bio_service.pb.h"
namespace BioContracts
{
	class IBioDatabaseClient
	{
	public:
		virtual std::shared_ptr<BioService::DatabaseFacialImageResponse>
			      AddFacialImage(const BioService::FacialImage& image) = 0;
	
	};


}

#endif
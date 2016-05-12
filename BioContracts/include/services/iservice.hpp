#ifndef IService_INCLUDED
#define IService_INCLUDED

#include "service_context.hpp"

namespace BioContracts
{
	class IService
	{
	public:
		virtual void registerService(BioServiceContext& builder) = 0;
		virtual void start() = 0;
		virtual void stop()  = 0;
	};
}

#endif
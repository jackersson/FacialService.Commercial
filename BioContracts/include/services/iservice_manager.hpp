#ifndef IServiceManager_INCLUDED
#define IServiceManager_INCLUDED

#include "iservice_configuration.hpp"

namespace BioContracts
{
	class IServiceManager
	{
	public:
		virtual void start(IServiceConfiguration& configuration) = 0;
		virtual void stop () = 0;
	};
}

#endif
#ifndef IServiceConfiguration_INCLUDED
#define IServiceConfiguration_INCLUDED

#include <string>
namespace BioContracts
{
	class IServiceConfiguration
	{
	public:
		virtual ~IServiceConfiguration() {}

		virtual const std::string&  ipAddress() const  = 0;
		virtual const unsigned int port() const  = 0;
		virtual const std::string&  formattedAddress() const = 0;
	};
}

#endif
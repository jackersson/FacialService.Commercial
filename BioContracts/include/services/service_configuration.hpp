#ifndef ServiceConfiguration_INCLUDED
#define ServiceConfiguration_INCLUDED

#include "iservice_configuration.hpp"
#include <sstream>
#include <iostream>
namespace BioContracts
{
	class ServiceConfiguration : public IServiceConfiguration
	{
	public:
		ServiceConfiguration( std::string  ip_address
			                  , unsigned int current_port)
			                  : ip_address_(ip_address)
			                  , port_(current_port)
		{
			std::stringstream stream;
			stream << ip_address << ":" << current_port;
			formatted_address_ = stream.str();

			std::cout << formatted_address_ << std::endl;
	  }

		~ServiceConfiguration(){}

		const std::string& ipAddress() const{	return ip_address_;	}
		const unsigned int port() const {	return port_;}			
		const std::string& formattedAddress() const { return formatted_address_; }

	private :
		std::string  ip_address_;
		std::string  formatted_address_;
		unsigned int port_;

	};
}

#endif
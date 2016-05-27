#ifndef ClientsStarter_INCLUDED
#define ClientsStarter_INCLUDED

//#include "services/bio_service_manager.hpp"
#include "services/service_configuration.hpp"

//#include "clients/bio_clients_manager.hpp"

#include "recognition_engines_context.hpp"

namespace BioFacialService
{
	class ClientsStarter
	{
	public:
		ClientsStarter(){}
		~ClientsStarter(){}
		void start()
		{			
			//BioContracts::ServiceConfiguration config("127.0.0.1", 49055);
			//clients_manager_ = std::make_unique<BioGrpc::BioClientsManager>();
			//clients_manager_->start(config);
		}
	
	private:
		ClientsStarter(const ClientsStarter&);
		ClientsStarter& operator=(const ClientsStarter&);

	private:
		//std::unique_ptr<BioGrpc::BioClientsManager> clients_manager_;		
	};
}

#endif
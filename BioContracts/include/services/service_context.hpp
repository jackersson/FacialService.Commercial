#ifndef BioServiceContext_INCLUDED
#define BioServiceContext_INCLUDED

#include "iservice_manager.hpp"
#include <grpc++/grpc++.h>

namespace BioContracts
{

	class BioServiceContext
	{
	public:
		BioServiceContext( std::shared_ptr<grpc::ServerBuilder> builder
			               /*, std::shared_ptr<grpc::ThreadPoolInterface> treadPool*/)
			: server_builder_(builder)//, thread_pool_(treadPool)
		{	}

		//grpc::ThreadPoolInterface& threadPool()              { return *thread_pool_  ; }
		std::shared_ptr<grpc::ServerBuilder> serverBuilder() { return server_builder_; }
	private:
		//std::shared_ptr<grpc::ThreadPoolInterface>  thread_pool_;
		std::shared_ptr<grpc::ServerBuilder>        server_builder_;
	};
}

#endif
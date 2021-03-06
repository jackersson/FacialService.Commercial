#include "services/bio_service_manager.hpp"
#include "services/facial_service_impl.hpp"

#include "src/cpp/server/dynamic_thread_pool.h"

#include <iostream>

using grpc::ServerBuilder;

namespace BioGrpc
{

	BioServiceManager::BioServiceManager( BioContracts::IRecognitionEnginesContext& context )
	{
  	//if (context.hasFacialEngine())
		//const 
		std::cout << "here" << std::endl;
		std::shared_ptr<FacialServiceImpl> ptr = std::make_shared<FacialServiceImpl>(context.facial_engine());
		std::cout << "here 1" << std::endl;
		services_.push_back(ptr);
		std::cout << "here 2" << std::endl;
		//thread_pool_ = std::shared_ptr<grpc::ThreadPoolInterface>(
			         //                new grpc::DynamicThreadPool(MAX_THREAD_POOL_CAPACITY));
	}
	
	void BioServiceManager::start(BioContracts::IServiceConfiguration& configuration)
	{
		if (!hasServices())
			return;

		std::shared_ptr<ServerBuilder> builder(new ServerBuilder());
		builder->AddListeningPort(configuration.formattedAddress(), grpc::InsecureServerCredentials());

		BioContracts::BioServiceContext context(builder);
				
		for (auto it = services_.begin(); it != services_.end(); ++it)
			it->get()->registerService(context);
		
		server_ = builder->BuildAndStart();

		for (auto it = services_.begin(); it != services_.end(); ++it)
			(*it)->start();

		std::cout << "Server listening on " << configuration.formattedAddress() << std::endl;	
	}

	bool BioServiceManager::hasServices() const
	{		
		return services_.size() > 0;
	}

	void BioServiceManager::stop()
	{
		server_->Shutdown();
		for (auto it = services_.begin(); it != services_.end(); ++it)
			it->get()->stop();
		services_.clear();
	}

}
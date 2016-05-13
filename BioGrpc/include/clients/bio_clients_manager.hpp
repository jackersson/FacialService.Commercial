#ifndef BioClientsManager_INCLUDED
#define BioClientsManager_INCLUDED

#include <memory>

#include "iservice.hpp"
#include "iservice_manager.hpp"

#include "irecognition_engines_context.hpp"
#include "src/cpp/server/dynamic_thread_pool.h"

#include "bio_database_service_client.hpp"

namespace BioGrpc
{
	class BioClientsManager : public BioContracts::IServiceManager
	{
	public:
		BioClientsManager() : thread_pool_(new grpc::DynamicThreadPool(MAX_THREAD_POOL_CAPACITY)) {}
		~BioClientsManager() {}

		void start(BioContracts::IServiceConfiguration& configuration)
		{

			database_service_client_ = std::make_shared<BioDatabaseServiceClient>(
			                         	              new BioDatabaseServiceClient( configuration
				                                                                  , thread_pool_));
			database_service_client_->start();


			//std::string address = configuration.formattedAddress();
			//auto it = channels_.find(address);
		//	if (it != channels_.end())
			//	return;
		
			//std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
			//channels_.insert(std::pair<std::string, std::shared_ptr<grpc::Channel>>(address, channel));

		//	thread_pool_->Add(&BioDatabaseServiceClient::H)
		}
		void stop() {}

		bool hasServices() const
		{
			return true;
		}

		std::shared_ptr<BioContracts::IBioDatabaseClient> bioDatabaseClient()
		{
			return database_service_client_;
		}


	private:
		BioClientsManager(const BioClientsManager&);
		BioClientsManager& operator=(const BioClientsManager&);

	private:
		std::shared_ptr<grpc::ThreadPoolInterface>  thread_pool_;
		std::shared_ptr<BioDatabaseServiceClient>   database_service_client_;
		//std::map<std::string, std::shared_ptr<grpc::Channel>> channels_;
		
		//std::vector<std::shared_ptr<BioContracts::IService>> services_;

		const int MAX_THREAD_POOL_CAPACITY = 10;

		//		std::shared_ptr<BioService::BiometricFacialSevice::AsyncService>  service_;
	};
}

#endif
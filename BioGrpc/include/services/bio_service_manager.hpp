#ifndef BioServiceManager_INCLUDED
#define BioServiceManager_INCLUDED

#include <memory>

#include "services/iservice.hpp"
#include "services/iservice_manager.hpp"

#include "engine/irecognition_engines_context.hpp"


namespace BioGrpc
{
	class BioServiceManager  : public BioContracts::IServiceManager 
	{
	public:
		BioServiceManager(const BioContracts::IRecognitionEnginesContext& context);
		~BioServiceManager() {}

		void start(BioContracts::IServiceConfiguration& configuration);
		void stop();

		bool hasServices() const;

	private:
		BioServiceManager(const BioServiceManager&);
		BioServiceManager& operator=(const BioServiceManager&);

	private:
	//	std::shared_ptr<grpc::ThreadPoolInterface>  thread_pool_;
		std::unique_ptr<grpc::Server>               server_     ;

		std::vector<std::shared_ptr<BioContracts::IService>> services_;

	//	const int MAX_THREAD_POOL_CAPACITY = 100;

//		std::shared_ptr<BioService::BiometricFacialSevice::AsyncService>  service_;
	};
}

#endif
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
		explicit BioServiceManager(BioContracts::IRecognitionEnginesContext& context);
		virtual ~BioServiceManager() { BioServiceManager::stop(); }

		void start(BioContracts::IServiceConfiguration& configuration) override;
		void stop() override;

		bool hasServices() const;

	private:
		BioServiceManager(const BioServiceManager&);
		BioServiceManager& operator=(const BioServiceManager&);
	
		std::unique_ptr<grpc::Server> server_;

		std::vector<std::shared_ptr<BioContracts::IService>> services_;
	};
}

#endif
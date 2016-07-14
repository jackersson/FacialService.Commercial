#include "services/bio_service_manager.hpp"
#include "services/facial_service_impl.hpp"

#include "handlers/facial_enrollment_handler.hpp"
#include "handlers/facial_acquisition_handler.hpp"
#include "handlers\facial_verification_handler.hpp"
#include "handlers/create_population_handler.hpp"

#include "src/cpp/server/dynamic_thread_pool.h"

#include <grpc++/grpc++.h>
#include <handlers/facial_identification_handler.hpp>
using grpc::ServerBuilder;

namespace BioGrpc
{
	FacialServiceImpl::FacialServiceImpl( std::shared_ptr<BioContracts::IFacialEngine> facial_engine) 
	                                   	: facial_engine_(facial_engine)
																			
	{	
		std::cout << "here 3" << std::endl;
		thread_pool_ = std::make_shared<grpc::DynamicThreadPool>(MAX_THREAD_POOL_CAPACITY);
		std::cout << "here 4" << std::endl;
	}

	FacialServiceImpl::~FacialServiceImpl()
	{
		FacialServiceImpl::stop();
	}

	void FacialServiceImpl::registerService(BioContracts::BioServiceContext& context)
	{
		auto builder = context.serverBuilder();
		service_ = std::make_shared<BioService::BiometricFacialSevice::AsyncService>();
		
		builder->RegisterService(service_.get());
  
		addRpcHandler<FacialAcquisitionHandler>   (*thread_pool_, builder);
		addRpcHandler<FacialVerificationHandler>  (*thread_pool_, builder);
		addRpcHandler<FacialIdentificationHandler>(*thread_pool_, builder);
		addRpcHandler<CreatePopulationHandler>    (*thread_pool_, builder);
	}

	void FacialServiceImpl::start()
	{	
		for (auto it = handlers_.begin(); it != handlers_.end(); ++it)
			thread_pool_->Add(it->second);
	}


	void FacialServiceImpl::stop()
	{
		for (auto it = handlers_.begin(); it != handlers_.end(); ++it)
			it->first->Shutdown();
		handlers_.clear();
		//for (auto it = server_completion_queues_.begin(); it != server_completion_queues_.end(); ++it)
		//	(*it)->Shutdown();
	}
}
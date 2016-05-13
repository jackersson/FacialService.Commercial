#include "services/bio_service_manager.hpp"
#include "services/facial_service_impl.hpp"

#include "handlers/facial_enrollment_handler.hpp"
#include "handlers/facial_acquisition_handler.hpp"

#include "src/cpp/server/dynamic_thread_pool.h"

#include <grpc++/grpc++.h>
using grpc::ServerBuilder;

namespace BioGrpc
{
	FacialServiceImpl::FacialServiceImpl( std::shared_ptr<BioContracts::IFacialEngine> facial_engine) 
	                                   	: facial_engine_(facial_engine)
																			, thread_pool_(new grpc::DynamicThreadPool(MAX_THREAD_POOL_CAPACITY))
	{	
	}

	FacialServiceImpl::~FacialServiceImpl()
	{
		stop();
	}

	void FacialServiceImpl::registerService(BioContracts::BioServiceContext& context)
	{
		std::shared_ptr<ServerBuilder> builder = context.serverBuilder();
	  service_ = std::shared_ptr<BioService::BiometricFacialSevice::AsyncService>(
			            new BioService::BiometricFacialSevice::AsyncService());
		
		builder->RegisterService(service_.get());
  
		addRpcHandler<FacialAcquisitionHandler>(*thread_pool_, builder);
	}

	void FacialServiceImpl::start()
	{
		//addRpcHandler<FacialAcquisitionHandler>(context.threadPool(), builder);
		//addRpcHandler<FacialEnrollmentHandler >(context.threadPool(), builder);
		for (auto it = handlers_.begin(); it != handlers_.end(); ++it)
			thread_pool_->Add(it->second);
	}


	void FacialServiceImpl::stop()
	{
		for (auto it = handlers_.begin(); it != handlers_.end(); ++it)
			it->first->Shutdown();
		//for (auto it = server_completion_queues_.begin(); it != server_completion_queues_.end(); ++it)
		//	(*it)->Shutdown();
	}
}
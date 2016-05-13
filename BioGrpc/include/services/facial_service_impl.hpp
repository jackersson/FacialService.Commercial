#ifndef BioFacialService_INCLUDED
#define BioFacialService_INCLUDED

#include "protobufs/bio_service.grpc.pb.h"
#include "engine/ifacial_engine.hpp"
//#include "facial_engine.hpp"
#include "services/iservice.hpp"

namespace BioGrpc
{

	class FacialServiceImpl : public BioContracts::IService
	{
	public:

		typedef std::function<void()> HandleRpcCallback;
		typedef std::pair<std::shared_ptr<grpc::ServerCompletionQueue>, HandleRpcCallback> HandlerItem;
		typedef std::list<HandlerItem> HandlerItemsList;

		FacialServiceImpl(std::shared_ptr<BioContracts::IFacialEngine> facial_engine);		

		~FacialServiceImpl();
			
		void registerService(BioContracts::BioServiceContext& context);

		void start();

		void stop();
		
	private:
		template<typename RequestData>
		void HandleRpc(grpc::ServerCompletionQueue* queue)
		{			
			new RequestData(service_, queue, facial_engine_);
			void* tag;
			bool  ok;

			while (true)
			{
				try
				{
					queue->Next(&tag, &ok);
					if (ok)
						static_cast<RequestData*>(tag)->Proceed();
				}
				catch (std::exception& ex)
				{
					std::cout << ex.what() << std::endl;
				}
			}
			
		}

		template<typename RequestData>
		void addRpcHandler( grpc::ThreadPoolInterface& threadPool
			                , std::shared_ptr<grpc::ServerBuilder> builder)
		{		
			std::shared_ptr<grpc::ServerCompletionQueue> cq_(builder->AddCompletionQueue());		

			HandleRpcCallback callback = std::bind(&FacialServiceImpl::HandleRpc<RequestData>, this, cq_.get());
			handlers_.push_back(HandlerItem(cq_, callback));
			//cq_ = builder->AddCompletionQueue();

			//HandleRpc<FacialAcquisitionHandler>(cq_.get());
			//server_completion_queues_.push_back(builder->AddCompletionQueue());
			//threadPool.Add(std::bind(&FacialServiceImpl::HandleRpc<RequestData>
				                 //     , this
													//		, cq));
		}

	private:
		FacialServiceImpl(const FacialServiceImpl&);
		FacialServiceImpl& operator=(const FacialServiceImpl&);
		
	private:
		//std::unique_ptr<grpc::ServerCompletionQueue> cq_;
		//std::vector<std::unique_ptr<grpc::ServerCompletionQueue>>         server_completion_queues_;
		std::shared_ptr<BioService::BiometricFacialSevice::AsyncService>  service_;
		std::shared_ptr<grpc::ThreadPoolInterface>  thread_pool_;
		std::shared_ptr<BioContracts::IFacialEngine> facial_engine_;

		const int MAX_THREAD_POOL_CAPACITY = 10;

		HandlerItemsList handlers_;

		
	};
}

#endif
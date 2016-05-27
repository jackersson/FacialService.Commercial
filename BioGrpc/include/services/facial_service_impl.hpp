#ifndef BioFacialService_INCLUDED
#define BioFacialService_INCLUDED

#include "protobufs/bio_service.grpc.pb.h"
#include "engine/ifacial_engine.hpp"
#include "services/iservice.hpp"

namespace BioGrpc
{

	class FacialServiceImpl : public BioContracts::IService
	{
	public:

		typedef std::function<void()> HandleRpcCallback;
		typedef std::pair<std::shared_ptr<grpc::ServerCompletionQueue>, HandleRpcCallback> HandlerItem;
		typedef std::list<HandlerItem> HandlerItemsList;

		explicit FacialServiceImpl(std::shared_ptr<BioContracts::IFacialEngine> facial_engine);		

		virtual ~FacialServiceImpl();
			
		void registerService(BioContracts::BioServiceContext& context) override;

		void start() override;

		void stop() override;
		
	private:
		template<typename RequestData>
		void HandleRpc(grpc::ServerCompletionQueue* queue) const
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
		}

		FacialServiceImpl(const FacialServiceImpl&);
		FacialServiceImpl& operator=(const FacialServiceImpl&);
		

		std::shared_ptr<BioService::BiometricFacialSevice::AsyncService>  service_;
		std::shared_ptr<grpc::ThreadPoolInterface>  thread_pool_;
		std::shared_ptr<BioContracts::IFacialEngine> facial_engine_;

		const int MAX_THREAD_POOL_CAPACITY = 10;

		HandlerItemsList handlers_;		
	};
}

#endif
#ifndef BioDatabaseServiceClient_INCLUDED
#define BioDatabaseServiceClient_INCLUDED

#include "bio_service.grpc.pb.h"
#include "iservice.hpp"

#include "add_facial_image_call.hpp"
#include "ibio_database_client.hpp"

#include "src/cpp/server/dynamic_thread_pool.h"
#include <calls/add_facial_image_call.hpp>

namespace BioGrpc
{
	class BioDatabaseServiceClient : public BioContracts::IBioDatabaseClient
	{
	public:

		explicit 
		BioDatabaseServiceClient( BioContracts::IServiceConfiguration& configuration
	                         	, std::shared_ptr<grpc::ThreadPoolInterface> thread_pool)
														: thread_pool_(thread_pool)	   														
		{			
			std::string address = configuration.formattedAddress();
			channel_ = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
			stub_    = BioService::BiometricDatabaseService::NewStub(channel_);
		}

		~BioDatabaseServiceClient() {}

		
		void start()
		{
			if (active_)
				return;

			active_ = false;
			thread_pool_->Add(std::bind(&BioDatabaseServiceClient::HandleRpc, this));
		}
		

		bool active() const
		{
			return active_;
		}

		std::shared_ptr<BioService::DatabaseFacialImageResponse>
		AddFacialImage(const BioService::FacialImage& image)
		{
			AddFacialImageCall* call = new AddFacialImageCall;
			call->response_reader_ = stub_->AsyncAddFacialImage(&call->context_, image, &completion_queue_);

			call->response_reader_->Finish(&call->reply_, &call->status_, (void*)call);

			if (call->status_.ok())
			{
				std::shared_ptr<BioService::DatabaseFacialImageResponse>
					response(new BioService::DatabaseFacialImageResponse(call->reply_));
				return response;
			}
			else
				throw new std::exception("Rpc Failed");		
		}
				
	private:
		//template<typename DataCall>
		void HandleRpc()
		{		
			void* got_tag;
			bool  ok = false;

			active_ = true;

			while (completion_queue_.Next(&got_tag, &ok))
			{
				active_ = true;
				AddFacialImageCall* call = static_cast<AddFacialImageCall*>(got_tag);

				//GPR_ASSERT(ok);
				try
				{
					if (call->status_.ok())
					{
						std::cout << "Received" << std::endl;
					}
					else
						std::cout << "Rpc failed" << std::endl;
				}
				catch (std::exception& ex)
				{
					std::cout << ex.what() << std::endl;
				}

				delete call;
			}		

			active_ = false;
		}

		template<typename RequestData>
		void addRpcHandler( grpc::ThreadPoolInterface& threadPool
			                , std::shared_ptr<grpc::ServerBuilder> builder)
		{
		}

	private:
		BioDatabaseServiceClient(const BioDatabaseServiceClient&);
		BioDatabaseServiceClient& operator=(const BioDatabaseServiceClient&);

	private:
		grpc::CompletionQueue   completion_queue_;
		std::unique_ptr<BioService::BiometricDatabaseService::Stub> stub_;

		std::shared_ptr<grpc::ThreadPoolInterface> thread_pool_;
		std::shared_ptr<grpc::Channel> channel_;

		bool active_;

		

	};
}

#endif
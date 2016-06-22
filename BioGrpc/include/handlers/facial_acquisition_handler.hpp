#ifndef FacialAcquisitionHandler__INCLUDED
#define FacialAcquisitionHandler__INCLUDED

#include "engine/ifacial_engine.hpp"
#include "protobufs/bio_service.grpc.pb.h"
#include "utils/response_convertor.hpp"

namespace BioGrpc
{
	class FacialAcquisitionHandler
	{
		enum RequestStatus { CREATE, PROCESS, CAN_WRITE, START_FINISH, FINISH };
	public:
		FacialAcquisitionHandler( std::shared_ptr<BioService::BiometricFacialSevice::AsyncService> service
			                    , grpc::ServerCompletionQueue* completion_queue
			                    , std::shared_ptr<BioContracts::IFacialEngine> facial_engine)
			                    : service_(service)
			                    , server_completion_queue_(completion_queue)
			                    , responder_(&server_context_)
			                    , status_(CREATE)
			                    , facial_engine_(facial_engine)
		{
			Proceed();
		}

		void Proceed()
		{
			if (status_ == CREATE)
			{
				status_ = PROCESS;
				try
				{
					service_->RequestAcquire(  &server_context_, &request_
						                      , &responder_, server_completion_queue_
						                      , server_completion_queue_, this);
				}
				catch (std::exception& ex)
				{
					std::cout << ex.what() << std::endl;
				}
				std::cout << "proceed" << std::endl;
			}
			else if (status_ == PROCESS)
			{
				new FacialAcquisitionHandler(service_, server_completion_queue_, facial_engine_);
				processRequest();
			}			
			else
			{
				GPR_ASSERT(status_ == FINISH);
				delete this;
			}
		}

		void processRequest()
		{			
			std::string image_bytestring = request_.bytestring();

			BioContracts::RawImage image(image_bytestring, request_.id());

			auto start = clock();
			std::cout << " acquisition start time : " << start << std::endl;
			BioContracts::IImageInfoPtr resp =
				facial_engine_->acquire(image);

			std::cout << " acquisition done time : " << clock() - start << std::endl;
			start = clock();
			std::shared_ptr<BioService::PortraitCharacteristic>	portrait_characteristics;
			if (resp == nullptr)			
				portrait_characteristics = std::make_shared<BioService::PortraitCharacteristic>();
			else
			{
				ResponseConvertor convertor;
				portrait_characteristics = std::make_shared<BioService::PortraitCharacteristic>
				                                          	(*convertor.getPortraitCharacteristics(resp));
			}			
			
			
			status_ = FINISH;
			responder_.Finish(*portrait_characteristics, grpc::Status::OK, this);			

			std::cout << " finnish " << clock() - start << std::endl;
		}		

	private:
		FacialAcquisitionHandler(const FacialAcquisitionHandler&);
		FacialAcquisitionHandler& operator=(const FacialAcquisitionHandler&);


	private:
		std::shared_ptr<BioService::BiometricFacialSevice::AsyncService> service_;
		grpc::ServerCompletionQueue*                     server_completion_queue_;
		grpc::ServerContext                              server_context_;

		std::shared_ptr<BioContracts::IFacialEngine> facial_engine_;

		BioService::Photo                    request_;
		grpc::ServerAsyncResponseWriter<BioService::PortraitCharacteristic>    responder_;

		RequestStatus status_;
	};

}
#endif
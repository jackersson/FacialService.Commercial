#ifndef FacialEnrollmentHandler__INCLUDED
#define FacialEnrollmentHandler__INCLUDED

#include "engine/ifacial_engine.hpp"
#include "protobufs/bio_service.grpc.pb.h"
#include <grpc++/grpc++.h>



using BioService::EnrollmentFeedback;

class FacialEnrollmentHandler
{
	enum RequestStatus { CREATE, PROCESS, CAN_WRITE, START_FINISH, FINISH };
public:
	FacialEnrollmentHandler( BioService::BiometricFacialSevice::AsyncService* service
		                     , grpc::ServerCompletionQueue* completion_queue
		                     , std::shared_ptr<BioContracts::IFacialEngine> facial_engine )
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
			service_->RequestEnroll(  &server_context_, &request_
				                      , &responder_     , server_completion_queue_
				                      , server_completion_queue_, this);
		}
		else if (status_ == PROCESS)
		{
			new FacialEnrollmentHandler(service_, server_completion_queue_, facial_engine_);

		//	std::function<void(const EnrollmentFeedback& ef)> functor
			//	= std::bind(&FacialEnrollmentHandler::EnrollFeedbackNotifier, this, std::placeholders::_1);

			//BioService::CommandPhoto cm;
			//	photo_list = bio_model_->dataClient().PhotoSelect(cm);


			//bio_model_->threadPool()->Add(std::bind(&SmartBio::Detector::FRsdkClient::enroll
			//	, bio_model_->frsdkClient()
			//	, request_
				//, photo_list
			//	, functor));

			status_ = CAN_WRITE;
		}
		else if (status_ == CAN_WRITE){}
		else if (status_ == START_FINISH)
		{
			status_ = FINISH;
			//responder_.Finish(grpc::Status::OK, this);
		}
		else
		{
			//GPR_ASSERT(status_ == FINISH);
			delete this;
		}
	}

	void EnrollFeedbackNotifier(const EnrollmentFeedback& ef)
	{
		/*
		std::cout << "Progress :" << ef.progress() << std::endl;
		std::cout << "Eyes found :" << ef.eyesfound() << std::endl;
		std::cout << "Success : " << ef.success() << std::endl;

		std::cout << std::endl;

		responder_.Write(ef, this);

		if (ef.progress() == 100)
			status_ = START_FINISH;
			*/
	}

private:
	BioService::BiometricFacialSevice::AsyncService* service_                ;
	grpc::ServerCompletionQueue*                     server_completion_queue_;
	grpc::ServerContext                              server_context_         ;

	std::shared_ptr<BioContracts::IFacialEngine> facial_engine_;

	BioService::AcquiredData                             request_;
	grpc::ServerAsyncResponseWriter<EnrollmentFeedback>  responder_;

	RequestStatus status_;
};

#endif
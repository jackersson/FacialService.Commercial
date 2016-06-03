#ifndef FacialVerificationHandler__INCLUDED
#define FacialVerificationHandler__INCLUDED

#include "engine/ifacial_engine.hpp"
#include "protobufs/bio_service.grpc.pb.h"
#include "common\matches.hpp"
#include "utils\response_convertor.hpp"
#include <common/verification_result.hpp>

using BioService::FaceSearchResult;
namespace BioGrpc
{
	class FacialVerificationHandler
	{
		enum RequestStatus { CREATE, PROCESS, CAN_WRITE, START_FINISH, FINISH };
	public:
		FacialVerificationHandler( std::shared_ptr<BioService::BiometricFacialSevice::AsyncService> service
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
				service_->RequestVerifyFace(&server_context_, &request_
					, &responder_, server_completion_queue_
					, server_completion_queue_, this);
			}
			else if (status_ == PROCESS)
			{
				new FacialVerificationHandler(service_, server_completion_queue_, facial_engine_);
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
			std::string target_image_bytestring = request_.target_image().bytestring();
			BioContracts::RawImage target_image(target_image_bytestring, target_image_bytestring.size());

			std::string comparison_image_bytestring = request_.comparison_image().bytestring();
			BioContracts::RawImage comparison_image(comparison_image_bytestring, comparison_image_bytestring.size());

			BioContracts::VerificationResultPtr result =
				facial_engine_->verify(target_image, comparison_image);

			std::shared_ptr<FaceSearchResult>	proto_matches;
			if (result == nullptr)
				proto_matches = std::make_shared<FaceSearchResult>();			
			else
			{
				ResponseConvertor convertor;
				proto_matches = std::make_shared<FaceSearchResult>(*convertor.get_face_search_result(result));
			}

			status_ = FINISH;
			responder_.Finish(*proto_matches, grpc::Status::OK, this);
		}


	private:
		std::shared_ptr<BioService::BiometricFacialSevice::AsyncService> service_;
		grpc::ServerCompletionQueue*                     server_completion_queue_;
		grpc::ServerContext                              server_context_;

		std::shared_ptr<BioContracts::IFacialEngine> facial_engine_;

		BioService::VerificationData                             request_;
		grpc::ServerAsyncResponseWriter<FaceSearchResult>     responder_;

		RequestStatus status_;
	};
}

#endif
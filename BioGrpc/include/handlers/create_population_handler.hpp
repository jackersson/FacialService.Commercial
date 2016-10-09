#ifndef CreatePopulationHandler__INCLUDED
#define CreatePopulationHandler__INCLUDED

#include "engine/ifacial_engine.hpp"
#include "protobufs/bio_service.grpc.pb.h"
#include "common\matches.hpp"
#include "utils\response_convertor.hpp"
#include <common/identification_result.hpp>

using BioService::FaceSearchResponse;
namespace BioGrpc
{
	class CreatePopulationHandler
	{
		enum RequestStatus { CREATE, PROCESS, CAN_WRITE, START_FINISH, FINISH };
	public:
		CreatePopulationHandler(std::shared_ptr<BioService::BiometricFacialSevice::AsyncService> service
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
				service_->RequestCreatePopulation(&server_context_, &request_
					                              , &responder_, server_completion_queue_
					                              , server_completion_queue_, this);
			}
			else if (status_ == PROCESS)
			{
				new CreatePopulationHandler(service_, server_completion_queue_, facial_engine_);
				processRequest();
			}
			else
			{
				//GPR_ASSERT(status_ == FINISH);
				delete this;
			}
		}

		void processRequest()
		{			
			std::list<BioContracts::RawImage> comparison_images;
			for (auto proto_it = request_.photos().begin();
				proto_it != request_.photos().end(); ++proto_it)
			{
				std::string comparison_image_bytestring = (*proto_it).bytestring();
				BioContracts::RawImage comparison_image(comparison_image_bytestring, proto_it->id());
				comparison_images.push_back(comparison_image);
			}
			long population_id = -1;
			BioContracts::IImageInfoSet result;
			try
			{			
				result =	facial_engine_->create_population(comparison_images, population_id);
			}
			catch ( std::exception& exception )	{
				std::cout << " create population handler exception : " << exception.what() << std::endl;
			}

			std::cout << "create population " << std::endl;
			std::shared_ptr<BioService::PopulationCreationResponse>	response;
			if (result.size() <= 0)
				response = std::make_shared<BioService::PopulationCreationResponse>();
			else
			{
				ResponseConvertor convertor;
				response = std::make_shared<BioService::PopulationCreationResponse>
					             (*convertor.get_population_creation_response(result));
			}

			response->set_id(population_id);

			//std::cout << "identification done size = " << result->matches().size() << std::endl;
			status_ = FINISH;
			responder_.Finish(*response, grpc::Status::OK, this);
		}


	private:
		std::shared_ptr<BioService::BiometricFacialSevice::AsyncService> service_;
		grpc::ServerCompletionQueue*                     server_completion_queue_;
		grpc::ServerContext                              server_context_;

		std::shared_ptr<BioContracts::IFacialEngine> facial_engine_;

		BioService::PhotoList                        request_;
		grpc::ServerAsyncResponseWriter<BioService::PopulationCreationResponse>     responder_;

		RequestStatus status_;
	};
}

#endif#pragma once

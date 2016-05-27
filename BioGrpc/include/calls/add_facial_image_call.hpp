#ifndef AddFacialImageCall_INCLUDED
#define AddFacialImageCall_INCLUDED

#include "protobufs/bio_service.pb.h"
#include <grpc++\grpc++.h>

namespace BioGrpc
{
	struct AddFacialImageCall
	{
		BioService::DatabaseFacialImageResponse reply_;

		grpc::ClientContext context_;
		grpc::Status        status_ ;


		std::unique_ptr<grpc::ClientAsyncResponseReader<BioService::DatabaseFacialImageResponse>> response_reader_;
	};
}

#endif
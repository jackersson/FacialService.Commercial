#ifndef FacialServiceStarter_INCLUDED
#define FacialServiceStarter_INCLUDED

#include "services/bio_service_manager.hpp"
#include "services/service_configuration.hpp"

#include "recognition_engines_context.hpp"

namespace BioFacialService
{
	class FacialServiceStarter 
	{
	public:
		FacialServiceStarter(){}
		~FacialServiceStarter(){}
		void start()
		{
			std::string cfg_path = "C:\\FVSDK_9_1_1\\etc\\frsdk.cfg";
			
			engines_ = std::make_shared<RecognitionEnginesContext>();
			engines_->createFacialEngine(cfg_path);
			
			BioContracts::ServiceConfiguration config("0.0.0.0", 49065);
			std::cout << "begin working" << std::endl;
			service_manager_ = std::make_unique<BioGrpc::BioServiceManager>(*engines_);
			std::cout << "begin working" << std::endl;
			service_manager_->start(config);
			//std::cout << "start working" << std::endl;
		}

		void stop()
		{
			service_manager_->stop();
		}

	private:
		FacialServiceStarter(const FacialServiceStarter&);
		FacialServiceStarter& operator=(const FacialServiceStarter&);

	private:
		std::unique_ptr<BioGrpc::BioServiceManager> service_manager_;
		std::shared_ptr<RecognitionEnginesContext>   engines_;
	};
}

#endif
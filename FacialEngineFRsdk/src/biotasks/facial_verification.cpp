#include "biotasks/facial_verification.hpp"


using namespace FRsdkTypes;

namespace BioFacialEngine
{	
	/*
	bool FacialVerificationProcessor::init(FaceVacsConfiguration configuration)
	{
		try
		{
			processor_      = new FRsdk::Verification::Processor(*configuration);
			/score_mappings_ = new FRsdk::ScoreMappings(*configuration);

			fir_builder_ = std::make_shared<FirBilder>(configuration);
			return true;
		}
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}
	*/

	FacialVerification::FacialVerification(const std::string& configuration_filename)
	{
		try
		{
			FaceVacsConfiguration configuration(new FRsdk::Configuration(configuration_filename));
			init(configuration);
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}
	}


	FacialVerification::FacialVerification(FaceVacsConfiguration configuration)
	{
		init(configuration);
	}

	bool FacialVerification::init(FaceVacsConfiguration configuration)
	{
		try
		{
			load_balancer_ = std::make_shared<VerificationLoadBalancer>(configuration);
			return true;
		}
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}
	
}
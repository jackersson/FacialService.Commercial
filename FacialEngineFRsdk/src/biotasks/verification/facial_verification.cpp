#include "biotasks/verification/facial_verification.hpp"

using namespace FRsdkTypes;

namespace BioFacialEngine
{		
	FacialVerification::FacialVerification(const std::string& configuration_filename)
	{
		try
		{
			auto configuration(std::make_shared<FRsdk::Configuration>(configuration_filename));
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

	BioContracts::Matches FacialVerification::verify(VerificationPair pair)
	{
		auto ptr = create_verification_item(pair.first);
		ptr->verify(pair.second);
		return ptr->matches();
	}

	VerificationItemPtr FacialVerification::create_verification_item(FRsdkEntities::ImageInfoPtr image)
	{
		return VerificationItemPtr(std::make_shared<VerificationItem>(image, load_balancer_));
	}
	
}
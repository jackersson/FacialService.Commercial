#include "biotasks/facial_identification.hpp"


using namespace FRsdkTypes;

namespace BioFacialEngine
{
	FacialIdentification::FacialIdentification(const std::string& configuration_filename)
	{

		try
		{
			configuration_ = std::make_shared<FRsdk::Configuration>(configuration_filename);
			init(configuration_);
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}
	}


	FacialIdentification::FacialIdentification(FaceVacsConfiguration configuration)
	{
		configuration_ = configuration;
		init(configuration);
	}

	bool FacialIdentification::init(FaceVacsConfiguration configuration)
	{
		try
		{
			
			//load_balancer_ = std::make_shared<EnrollmentLoadBalancer>(configuration);
			return true;
		}
		catch (const FRsdk::FeatureDisabled& e) { std::cout << e.what(); return false; }
		catch (const FRsdk::LicenseSignatureMismatch& e) { std::cout << e.what(); return false; }
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}
}
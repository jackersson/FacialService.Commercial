#include "biotasks/identification/facial_identification.hpp"
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
			return true;
		}
		catch (const FRsdk::FeatureDisabled& e) { std::cout << e.what(); return false; }
		catch (const FRsdk::LicenseSignatureMismatch& e) { std::cout << e.what(); return false; }
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}

	BioContracts::Matches FacialIdentification::identify(IdentificationPair pair)
	{		
		auto object = pair.first;
		if (object->size() <= 0)
		{
			std::cout << "identify no faces detected" << std::endl;
			return BioContracts::Matches();
		}
	
		auto ident_item(std::make_shared<IdentificationItem>(pair.second, configuration_));
		//items_.push_back(ident_item);

		auto matches = ident_item->identify(pair.first);
		
		return matches;
	}
}
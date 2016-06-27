#include "biotasks/identification/facial_identification.hpp"
using namespace FRsdkTypes;

namespace BioFacialEngine
{
	sitmo::prng_engine FacialIdentification::randomizer_;

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

	IdentificationItemPtr FacialIdentification::create_population(std::list<FRsdkEntities::ImageInfoPtr> images)
	{
		long key = randomizer_();
		while (items_.find(key) != items_.end())		
			key = randomizer_();

		auto population = std::make_shared<IdentificationItem>(images, configuration_, key);
		items_.insert(std::pair<long, IdentificationItemPtr>(key, population));

		return population;
	}

	BioContracts::Matches FacialIdentification::identify(IdentificationPair pair, long population_id)
	{		
		auto object = pair.first;
		if (object->size() <= 0)
			return BioContracts::Matches(DEFAULT_POPULATION);

		auto population_info = items_.find(population_id);
		IdentificationItemPtr population;
		if (population_id == DEFAULT_POPULATION && population_info == items_.end())		
			population = create_population(pair.second);
		else		
			population = population_info->second;

		auto matches = population->identify(pair.first);
		
		return matches;
	}
}
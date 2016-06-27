#ifndef FacialIdentification_INCLUDED
#define FacialIdentification_INCLUDED

#include "identification_item.hpp"
#include "utils/prng_engine.hpp"

namespace BioFacialEngine
{
	typedef std::pair<FRsdkEntities::ImageInfoPtr, std::list<FRsdkEntities::ImageInfoPtr>> IdentificationPair;
	class FacialIdentification
	{
	public:
		explicit FacialIdentification(const std::string& configuration_filename);
		explicit FacialIdentification(FRsdkTypes::FaceVacsConfiguration configuration);

		BioContracts::Matches identify(IdentificationPair pair, long population_id = DEFAULT_POPULATION);

		IdentificationItemPtr create_population(std::list<FRsdkEntities::ImageInfoPtr> images);
	
	private:
		FacialIdentification(const FacialIdentification&);
		FacialIdentification const & operator=(FacialIdentification const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

		FRsdkTypes::FaceVacsConfiguration configuration_;
		
		std::map<long, IdentificationItemPtr> items_;

		static sitmo::prng_engine randomizer_;
		static const long DEFAULT_POPULATION = -1;
	};

	typedef std::shared_ptr<FacialIdentification> FacialIdentificationPtr;

}

#endif
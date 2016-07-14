#ifndef FacialIdentification_INCLUDED
#define FacialIdentification_INCLUDED

#include "identification_item.hpp"
#include "utils/prng_engine.hpp"

namespace BioFacialEngine
{
	class IdentificationPair
	{
	public:
		IdentificationPair( FRsdkEntities::ImageInfoPtr target_image
		                 	/*, const std::list<FRsdkEntities::ImageInfoPtr>& population	*/) 
			                : target_image_(target_image)
			                //, population_(population)
			                , pupulation_id_(DEFAULT_POPULATION)
		{	}

		IdentificationPair( FRsdkEntities::ImageInfoPtr target_image, long population_id)
		                 	: target_image_(target_image), pupulation_id_(population_id)
		{	}

		FRsdkEntities::ImageInfoPtr target_image() const {
			return target_image_;
		}

		void add_population_image(FRsdkEntities::ImageInfoPtr image)	{
			population_.push_back(image);
		}
		
		const std::list<FRsdkEntities::ImageInfoPtr>& population() const {		
			return population_;
		}				

		int population_id() const	{					
			return pupulation_id_;
		}

		static const long DEFAULT_POPULATION = -1;

	private:
		FRsdkEntities::ImageInfoPtr target_image_;
		std::list<FRsdkEntities::ImageInfoPtr> population_;
		long pupulation_id_;

		//static const long DEFAULT_POPULATION = -1;

	};

	//typedef std::shared_ptr<IdentificationPair> IdentificationPairPtr;
	//typedef std::pair<FRsdkEntities::ImageInfoPtr, std::list<FRsdkEntities::ImageInfoPtr>> IdentificationPair;
	class FacialIdentification
	{
	public:
		explicit FacialIdentification(const std::string& configuration_filename);
		explicit FacialIdentification(FRsdkTypes::FaceVacsConfiguration configuration);

		BioContracts::Matches identify(const IdentificationPair& pair);

		IdentificationItemPtr create_population(std::list<FRsdkEntities::ImageInfoPtr> images);
	
	private:
		FacialIdentification(const FacialIdentification&);
		FacialIdentification const & operator=(FacialIdentification const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

		FRsdkTypes::FaceVacsConfiguration configuration_;
		
		std::map<long, IdentificationItemPtr> items_;

		static sitmo::prng_engine randomizer_;
	};

	typedef std::shared_ptr<FacialIdentification> FacialIdentificationPtr;

}

#endif
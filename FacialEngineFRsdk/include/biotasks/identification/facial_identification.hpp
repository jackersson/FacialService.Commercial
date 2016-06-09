#ifndef FacialIdentification_INCLUDED
#define FacialIdentification_INCLUDED

#include "identification_item.hpp"

namespace BioFacialEngine
{
	typedef std::pair<FRsdkEntities::ImageInfoPtr, std::list<FRsdkEntities::ImageInfoPtr>> IdentificationPair;
	class FacialIdentification
	{
	public:
		explicit FacialIdentification(const std::string& configuration_filename);
		explicit FacialIdentification(FRsdkTypes::FaceVacsConfiguration configuration);

		BioContracts::Matches identify(IdentificationPair pair);

	private:
		FacialIdentification(const FacialIdentification&);
		FacialIdentification const & operator=(FacialIdentification const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

		FRsdkTypes::FaceVacsConfiguration configuration_;
		
		std::list<IdentificationItemPtr> items_;
	};

	typedef std::shared_ptr<FacialIdentification> FacialIdentificationPtr;

}

#endif
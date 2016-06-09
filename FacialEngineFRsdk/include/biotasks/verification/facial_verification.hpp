#ifndef FacialVerification_INCLUDED
#define FacialVerification_INCLUDED

#include "verification_item.hpp"

namespace BioFacialEngine
{		
	typedef std::pair<FRsdkEntities::ImageInfoPtr, FRsdkEntities::ImageInfoPtr> VerificationPair;
	class FacialVerification
	{
	public:
		explicit FacialVerification(const std::string& configuration_filename);		
		explicit FacialVerification(FRsdkTypes::FaceVacsConfiguration configuration);

		BioContracts::Matches verify(VerificationPair pair);

	private:
		FacialVerification(const FacialVerification&);
		FacialVerification const & operator=(FacialVerification const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);
		VerificationItemPtr create_verification_item(FRsdkEntities::ImageInfoPtr image);

		VerificationLoadBalancerPtr load_balancer_;
	};

	typedef std::shared_ptr<FacialVerification> FacialVerificationPtr;

}

#endif
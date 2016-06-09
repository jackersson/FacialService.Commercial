#ifndef IdentificationItem_INCLUDED
#define IdentificationItem_INCLUDED

#include "biotasks\ifir_builder.hpp"
#include "common\matches.hpp"
#include <wrappers/face_info.hpp>
#include <wrappers/image_info.hpp>
#include "identification_load_balancer.hpp"


namespace BioFacialEngine
{
	class IdentificationItem
	{
		typedef std::pair<FRsdkTypes::FaceVacsImage, long>  VerificationObject;
		typedef std::pair<FRsdk::FIR, long>  VerificationSubject;
	public:
		explicit IdentificationItem( const std::list<FRsdkEntities::ImageInfoPtr>& images
			                         , FRsdkTypes::FaceVacsConfiguration configuration);
			

		BioContracts::Matches identify(FRsdkEntities::ImageInfoPtr image);
		
		void clear();

		//BioContracts::MatchesPtr matches() const;

		//TODO remove when not need
		int counter() const;

		//size_t size() const;

	private:
		void init(const std::list<FRsdkEntities::ImageInfoPtr>& images);

		IdentificationItem(const IdentificationItem&);
		IdentificationItem const & operator=(IdentificationItem const&);

		std::list<FRsdkEntities::FaceInfoPtr> container_    ;
		IdentificationLoadBalancerPtr         load_balancer_;

		//FirBuilderRef                  fir_builder_;
		FRsdkTypes::FaceVacsConfiguration configuration_;

	};
	typedef std::shared_ptr<IdentificationItem> IdentificationItemPtr;
}
#endif
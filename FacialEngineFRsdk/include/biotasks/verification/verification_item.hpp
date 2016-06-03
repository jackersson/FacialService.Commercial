#ifndef VerificationItem_INCLUDED
#define VerificationItem_INCLUDED

#include <wrappers/image_info.hpp>
#include "verification_load_balancer.hpp"

namespace BioFacialEngine
{
	class VerificationItem
	{
		typedef std::pair<FRsdkTypes::FaceVacsImage, long>  VerificationObject;
		typedef std::pair<FRsdk::FIR, long>  VerificationSubject;

	public:
		explicit VerificationItem( FRsdkEntities::ImageInfoPtr object
			                       , VerificationLoadBalancerPtr load_balancer);

		const BioContracts::Matches& verify(FRsdkEntities::ImageInfoPtr object);

		void process(FRsdkEntities::FaceInfoPtr item);

		void clear();

		BioContracts::Matches matches() const;

		//TODO remove when not need
		int counter() const;

		size_t size() const;

	private:
		VerificationItem(const VerificationItem&);
		VerificationItem const & operator=(VerificationItem const&);

		std::list<FRsdkEntities::FaceInfoPtr> container_;
		BioContracts::Matches matches_;

		VerificationLoadBalancerPtr    load_balancer_;		
		//FirBuilderRef                  fir_builder_  ;

		int counter_;
	};
	typedef std::shared_ptr<VerificationItem> VerificationItemPtr;
}
#endif
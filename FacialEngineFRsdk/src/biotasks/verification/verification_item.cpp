#include "biotasks/verification/verification_item.hpp"

using namespace concurrency;
using namespace FRsdkTypes;
using namespace FRsdkEntities;

namespace BioFacialEngine
{
	
	VerificationItem::VerificationItem( ImageInfoPtr object
		                                , VerificationLoadBalancerPtr load_balancer)
																		: load_balancer_(load_balancer)
		                                , counter_(0)
	{
		parallel_for_each(object->cbegin(), object->cend(),
			[&](FRsdkEntities::FaceInfoPtr face){
			container_.push_back(face);
		});
	}

	const BioContracts::Matches& VerificationItem::verify(ImageInfoPtr object)
	{
		parallel_for_each(object->cbegin(), object->cend(),
			[&](FRsdkEntities::FaceInfoPtr face) {
		  	process(face);
		});

		return matches_;
	}

	void VerificationItem::process(FaceInfoPtr item)
	{
		if (!item->has_fir())
			return;

		VerificationObject  verification_object(item->extracted_image(), item->id());

		concurrency::parallel_for_each(container_.cbegin(), container_.cend(),
			[&](FRsdkEntities::FaceInfoPtr iter_item)
		{
			//if (item->image_id() == iter_item->image_id())
			//return;

			++counter_;

			auto match_score = load_balancer_->verify(iter_item->extracted_image(), item->enrollment_data()->fir());

			if (match_score > 0.5f)
				matches_.add(item->id(), BioContracts::Match(iter_item->id(), match_score));
		}
		);
	}

	void VerificationItem::clear()	{
		container_.clear();
		matches_.clear();
	}

	BioContracts::Matches VerificationItem::matches() const	{
		return matches_;
	}

	//TODO remove when not need
	int VerificationItem::counter() const	{
		return counter_;
	}

	size_t VerificationItem::size() const	{
		return matches_.size();
	}
}
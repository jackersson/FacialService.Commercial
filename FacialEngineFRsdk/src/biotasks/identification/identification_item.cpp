#include "biotasks/identification/identification_item.hpp"

using namespace concurrency;
using namespace FRsdkTypes;
using namespace FRsdkEntities;
using namespace BioContracts;

namespace BioFacialEngine
{	
	IdentificationItem::IdentificationItem( const std::list<ImageInfoPtr>& images
			                                  , FaceVacsConfiguration configuration)
			                                  : load_balancer_(nullptr)
			                                  //, matches_(std::make_shared<Matches>())
			                                  , configuration_(configuration)			                             
	{
		init(images);
	}

	Matches IdentificationItem::identify(ImageInfoPtr image)
	{
		Matches matches_;
		parallel_for_each(image->cbegin(), image->cend(),
			[&](FaceInfoPtr face)
		  {
		  	auto matches = load_balancer_->identify(face->extracted_image());
		  	matches_.add(face->id(), matches);
		  	container_.push_back(face);
		  }
		);
		load_balancer_->stop();
		return matches_;
	}

	void IdentificationItem::clear()	{
		container_.clear();	
	}
	
	void IdentificationItem::init(const std::list<ImageInfoPtr>& images)
	{	
		if (images.size() <= 0)
		{
			std::cout << "IdentificationItem::init no images for population" << std::endl;
			return;
		}

		auto population(std::make_shared<FRsdk::Population>(*configuration_));
		size_t size = 0;
		for (auto image_it = images.cbegin(); image_it != images.cend(); ++image_it)
		{
			auto image = *image_it;
			for (auto face_it = image->cbegin(); face_it != image->cend(); ++face_it)
			{
				auto face = *face_it;
				auto fir = face->enrollment_data()->fir();		
				if (face->enrollment_data()->good() && fir != nullptr)
				{
					population->append(*fir, std::to_string((face->id())));
				  ++size;
		  	}
				else 
					std::cout << "IdentificationItem::init Fir Null" << std::endl;
			}			
		}			
		
		if (size <= 0)
		{
			std::cout << "IdentificationItem::init size == 0" << std::endl;
			return;
		}
	
		load_balancer_ = std::make_shared<IdentificationLoadBalancer>(configuration_, population);
	}
	
}
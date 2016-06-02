#ifndef IdentificationResult_INCLUDED
#define IdentificationResult_INCLUDED

#include "common\matches.hpp"
#include "common\iimage_characteristic.hpp"
#include <list>

namespace BioContracts
{
	class IdentificationResult
	{
	public:
		IdentificationResult( const Matches& matches			                  
			                  , IImageInfoPtr main_image   )
			                  : matches_(matches)
												//, main_image_(main_image)
		{
			images_.push_back(main_image);
		}

		const Matches& matches() const { return matches_; }

		template <typename Iter>
		void set_identification_images(Iter begin, Iter end)
		{
			Concurrency::parallel_for_each(begin, end,
				[&](IImageInfoPtr image)
			  {
			  	images_.push_back(image);
			  }				
			);
		}

		std::list<IImageInfoPtr>::const_iterator cbegin() const{
			return images_.cbegin();
		}

		std::list<IImageInfoPtr>::const_iterator cend() const{
			return images_.cend();
		}
	
	private:
		Matches matches_;
		//IImageInfoPtr main_image_;
		std::list<IImageInfoPtr> images_;
	};

	typedef std::shared_ptr<IdentificationResult> IdentificationResultPtr;
}

#endif
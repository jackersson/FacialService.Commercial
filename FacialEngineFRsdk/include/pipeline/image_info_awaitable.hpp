#ifndef ImageInfoAwaitable_INCLUDED
#define ImageInfoAwaitable_INCLUDED

#include "pipeline/face_info_awaitable.hpp"

namespace Pipeline
{
	
	class ImageInfoAwaitable : public PipelineAwaitable<FRsdkEntities::ImageInfoPtr>
	                       	 , public std::enable_shared_from_this<ImageInfoAwaitable>
	{		
		typedef std::list<FaceInfoAwaitablePtr>    FaceInfoAwaitableSet;
	public:
		explicit ImageInfoAwaitable( FRsdkEntities::ImageInfoPtr ptr, long task)
		                           : PipelineAwaitable(ptr), task_(task)
		{
			
		}		

		FaceInfoAwaitableSet::const_iterator cbegin() const{
			return awaitables_.cbegin();
		}

		FaceInfoAwaitableSet::const_iterator cend() const{
			return awaitables_.cend();
		}

		void init()
		{
			auto this_ptr(shared_from_this());
			Concurrency::parallel_for_each(item_->cbegin(), item_->cend(),
				[&](FRsdkEntities::FaceInfoPtr ptr)
			  {				
			  	FaceInfoAwaitablePtr face_awaitable(new FaceInfoAwaitable(this_ptr, ptr, task_));
			  	awaitables_.push_back(face_awaitable);
			  }
			);
		}

	private:	
		ImageInfoAwaitable(const ImageInfoAwaitable&);
		ImageInfoAwaitable const & operator=(ImageInfoAwaitable const&);

	
		long task_;
		FaceInfoAwaitableSet awaitables_;
	};

	
}
#endif

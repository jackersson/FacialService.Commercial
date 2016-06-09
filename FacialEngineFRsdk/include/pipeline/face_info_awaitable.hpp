#ifndef FaceInfoAwaitable_INCLUDED
#define FaceInfoAwaitable_INCLUDED

#include <wrappers/image_info.hpp>
#include "pipeline/pipeline_item_holders.hpp"
#include <mutex>

namespace Pipeline
{
	class ImageInfoAwaitable;
	typedef std::shared_ptr<ImageInfoAwaitable> ImageInfoAwaitablePtr;

	class FaceInfoAwaitable
	{
		typedef std::pair<FRsdkEntities::FaceInfoPtr, long> FaceInfoTaskItem;
	public:
		explicit FaceInfoAwaitable( ImageInfoAwaitablePtr parent
			                        , FRsdkEntities::FaceInfoPtr item
			                        , long task);
															

		clock_t start_time() const {
			return start_time_;
		}

		FRsdkEntities::FaceInfoPtr item() const	{
			return task_item_.first;
		}

		bool has_task(BiometricTask task) const {
			return (task_item_.second & task) == task;
		}

		void set_done(BiometricTask task)	{
			std::lock_guard<std::mutex> lock(mutex_);
			finished_tasks_ |= task;
		}

		bool done() const;

	private:
		bool check_if_undone( bool flag ) const {
			++done_query_count_;
			auto problem = !flag && done_query_count_ >= MAX_QUERY_COUNT;
			
			if (problem)			
			  std::cout << "not done problem" << std::endl;			

			return problem;
		}

		FaceInfoAwaitable(const FaceInfoAwaitable&);
		FaceInfoAwaitable const & operator=(FaceInfoAwaitable const&);

		FaceInfoTaskItem         task_item_;
		ImageInfoAwaitablePtr    parent_   ;

		mutable size_t done_query_count_;

		long finished_tasks_;

		std::mutex mutex_;

		clock_t start_time_;

		static const size_t MAX_QUERY_COUNT = 3;
	};

	typedef std::shared_ptr<FaceInfoAwaitable> FaceInfoAwaitablePtr;

}
#endif

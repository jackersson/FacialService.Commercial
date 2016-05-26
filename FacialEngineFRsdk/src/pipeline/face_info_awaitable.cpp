#include "pipeline/face_info_awaitable.hpp"
#include "pipeline/image_info_awaitable.hpp"

namespace Pipeline
{
	FaceInfoAwaitable::FaceInfoAwaitable( ImageInfoAwaitablePtr parent
			                                , FRsdkEntities::FaceInfoPtr item
			                                , long task) 
															        : task_item_(item, task)
			                                , parent_(parent)
			                                , done_query_count_(0)
			                                , finished_tasks_(0)
			                                , start_time_(clock())
		{
			set_done(FaceFind);
			parent->wait_available_working_unit();
		}


	bool FaceInfoAwaitable::done() const {
		auto result = finished_tasks_ == task_item_.second;

		auto done_request_count_problem = check_if_undone(result);
	
		if (done_request_count_problem || result)
			parent_->free_unit();

		return result;
	}
}
#include "pipeline/biometric_pipeline_load_balanced.hpp"
#include <pipeline/image_info_awaitable.hpp>

using namespace Concurrency;
using namespace FRsdkEntities;

namespace Pipeline
{
	typedef call<FaceInfoAwaitablePtr> FaceCall;
	void BiometricPipelineBalanced::initialize()
	{
		facial_image_extractor_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		  {
		  	this->extract_facial_image(pInfo);
		  	if (pInfo->has_task(BiometricTask::Enrollment))
		  		asend(enrollment_multiplex_buffer_, pInfo);
		  	else
		  		asend(*finish_stage_, pInfo);
		  }
		);

		face_analyzer_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		  {
		  	this->portrait_analyze(pInfo);
		  	if (pInfo->has_task(BiometricTask::IsoComplianceTest))
		  		asend(*iso_compliance_test_, pInfo);
		  	else
		  		asend(*finish_stage_, pInfo);
		  }
		);

		for (auto i = 0; i < MAX_ENROLMENT_BRANCHES_COUNT; ++i)
		{
			enrollment_processors_[i] = std::make_unique<FaceCall>(
				[this, i](FaceInfoAwaitablePtr pInfo)
			{
				this->enroll(pInfo);
				asend(*enrollment_multiplexer_, pInfo);
			},
				[this, i](FaceInfoAwaitablePtr pInfo)->bool
			{
				if ((nullptr != pInfo) && (i != enrollment_next_filter_id_))
					return false;
				++enrollment_next_filter_id_;
				if (enrollment_next_filter_id_ >= MAX_ENROLMENT_BRANCHES_COUNT)
					enrollment_next_filter_id_ = 0;
				return true;
			}
			);
			enrollment_multiplex_buffer_.link_target(enrollment_processors_[i].get());
		}
		
		enrollment_multiplexer_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		{
			enrollment_multiplex_queue_.push(pInfo);
			while ((enrollment_multiplex_queue_.size() > 0))
			{
				// asend(m_multiplexBuffer, m_multiplexQueue.top());
				FaceInfoAwaitablePtr item = enrollment_multiplex_queue_.top();
				if (item->has_task(BiometricTask::Verification))
					asend(*verification_processor_, pInfo);
				else
					asend(*finish_stage_, pInfo);

				enrollment_multiplex_queue_.pop();
			}
		}
		);



		verification_processor_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		{
			this->verify(pInfo->item());
			asend(*finish_stage_, pInfo);
		}
		);

		iso_compliance_test_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		{
			this->iso_compliance_test(pInfo);
			asend(*finish_stage_, pInfo);
		}
		);

		finish_stage_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		{
			if (pInfo->done())
				governor_.free_unit();
		}
		);
	}

	ImageInfoPtr BiometricPipelineBalanced::acquire(const std::string& filename, long task)
	{
		if (task & Verification)
			throw std::exception("cannot acquire only for verification task");

		TaskInfo workItem(filename, task);
		return push(workItem);
	}


	ImageInfoPtr BiometricPipelineBalanced::push(TaskInfo task_info)
	{
		auto image_info = this->load_image(task_info.first);

		if (nullptr == image_info)
			return nullptr;

		this->face_find(image_info);

		if (image_info->size() <= 0)
			return nullptr;

		auto task = task_info.second;
		auto image_awaitable = std::make_shared<ImageInfoAwaitable>(image_info, task);
		image_awaitable->init();
		parallel_for_each(image_awaitable->cbegin(), image_awaitable->cend(),
	    [&](FaceInfoAwaitablePtr face)
		  {		  	
		  	governor_.wait_available_working_unit();
		  	if (face->has_task(PortraitAnalysis))
		  		asend(*face_analyzer_, face);
		  
		  	if (face->has_task(FaceImageExtraction))
		  		asend(*facial_image_extractor_, face);
		  }
		);

		image_awaitable->wait_until_empty();

		return image_info;
	}
	/*
	void BiometricPipelineBalanced::verify(const std::string object, const std::string subject, bool fast = false)
	{
		//auto object_configuration = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		//auto subject_configuration = object_configuration | BiometricTask::Enrollment;

		//std::vector<BioWorkItem> items;
		//items.push_back(BioWorkItem(object, object_configuration));
		//items.push_back(BioWorkItem(subject, subject_configuration));

		//push(items);
	}

	void BiometricPipelineBalanced::identify(const std::string& object, const std::vector<std::string>& subjects)
	{

	}
	*/

	void BiometricPipelineBalanced::stop()
	{
		governor_.wait_until_empty();
		done();
	}
}
#include "pipeline/biometric_pipeline_load_balanced.hpp"
#include <pipeline/image_info_awaitable.hpp>
#include <common/raw_image.hpp>
#include <biotasks/facial_identification.hpp>

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
				auto item = enrollment_multiplex_queue_.top();
				//if (item->has_task(Verification))
					//asend(*verification_processor_, pInfo);
				//else
					asend(*finish_stage_, pInfo);

				enrollment_multiplex_queue_.pop();
			}
		}
		);


		/*
		verification_processor_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		  {
		  	this->verify(pInfo->item());
		  	asend(*finish_stage_, pInfo);
		  }
		);
		*/
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

		BioFacialEngine::FaceVacsIOUtils utils;
		auto image = utils.loadFromFile(filename);

		if (image == nullptr)
			return nullptr;

		TaskInfo workItem(image, task);
		return push(workItem);
	}

	ImageInfoPtr BiometricPipelineBalanced::acquire(const BioContracts::RawImage& raw_image, long task)
	{
		if (task & Verification)
			throw std::exception("cannot acquire only for verification task");

		BioFacialEngine::FaceVacsIOUtils utils;
		auto image = utils.loadFromBytes(raw_image.bytes(), raw_image.size(), BioService::ImageFormat::JPEG);
		
		if (image == nullptr)
			return nullptr;

		TaskInfo workItem(image, task);
		return push(workItem);
	}


	ImageInfoPtr BiometricPipelineBalanced::push(TaskInfo task_info)
	{
		auto image_info = std::make_shared<ImageInfo>(task_info.first);

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

	BioContracts::VerificationResultPtr
	BiometricPipelineBalanced::verify_face(const std::string& object, const std::string& subject, bool fast)
	{
		auto object_configuration  = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_configuration = object_configuration | Enrollment;
		//BioContracts::VerificationResult result = new 

		BioFacialEngine::VerificationPair images;
		BioFacialEngine::FaceVacsIOUtils utils;		
		parallel_invoke(
			[&]()
		  {
		  	auto image = utils.loadFromFile(object);
		  	TaskInfo workItem(image, object_configuration);		 
				images.first  = push(workItem);
		  },
			[&]()
		  {
		  	auto image = utils.loadFromFile(subject);
		  	TaskInfo workItem(image, subject_configuration);
				images.second = push(workItem);
		  }
		);

		auto result = this->verify(images);
		return result;
		//push(items);
	}

	BioContracts::IdentificationResultPtr
  BiometricPipelineBalanced::identify_face( const std::string& object
		                                      , const std::vector<std::string>& subjects
																		      , bool  fast )
	{
		auto object_configuration  = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_configuration = object_configuration | Enrollment;

		BioFacialEngine::IdentificationPair images;
		BioFacialEngine::FaceVacsIOUtils utils;
		parallel_invoke(
			[&]()
		{
			auto image = utils.loadFromFile(object);
			TaskInfo workItem(image, object_configuration);
			images.first = push(workItem);
		},
			[&]()
		  {
			parallel_for_each(subjects.begin(), subjects.end(),
				[&](const std::string& item)
			  {
					auto image = utils.loadFromFile(item);
				  TaskInfo workItem(image, subject_configuration);
				  images.second.push_back(push(workItem));
			  });		
		  }
		);

		auto result = this->identify(images);
		return result;

	}
	

	void BiometricPipelineBalanced::stop()
	{
		governor_.wait_until_empty();
		done();
	}
}
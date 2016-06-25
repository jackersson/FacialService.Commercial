#include "pipeline/biometric_pipeline_load_balanced.hpp"
#include <pipeline/image_info_awaitable.hpp>
#include <common/raw_image.hpp>
#include <biotasks/identification/facial_identification.hpp>
#include <utils/face_vacs_io_utils.hpp>

using namespace Concurrency;
using namespace FRsdkEntities;
using namespace BioContracts;

namespace Pipeline
{
	BiometricPipelineBalanced::BiometricPipelineBalanced( IBiometricProcessorPtr pipeline)
			                                                : BiometricPipelineAgent(pipeline)
			                                                , governor_(MAX_PIPELINE_SLOT_COUNT)
			                                                , face_finder_(nullptr)
			                                                , facial_image_extractor_(nullptr)
			                                                , face_analyzer_(nullptr)
			                                                , iso_compliance_test_(nullptr)			                               
			                                                , finish_stage_(nullptr)
			                                                , enrollment_next_filter_id_(0)
	{
		queue_sizes.resize(5, 0);
		enrollment_processors_.resize(MAX_ENROLMENT_BRANCHES_COUNT);

		initialize();
	}


	typedef call<FaceInfoAwaitablePtr> FaceCall;
	void BiometricPipelineBalanced::initialize()
	{
		facial_image_extractor_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		  {
		  	this->extract_facial_image(pInfo);
		  	if (pInfo->has_task(Enrollment))
		  		asend(enrollment_multiplex_buffer_, pInfo);
		  	else
		  		asend(*finish_stage_, pInfo);
		  }
		);

		face_analyzer_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		  {
		  	this->portrait_analyze(pInfo);
		  	if (pInfo->has_task(IsoComplianceTest))
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
				auto item = enrollment_multiplex_queue_.top();			
				asend(*finish_stage_, pInfo);
				enrollment_multiplex_queue_.pop();
			}
		});
		
		
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
				{
					governor_.free_unit();
					std::cout << "done " << pInfo->item()->id() << std::endl;
				}
				else
					std::cout << "not done" << std::endl;
		  });
	}

	ImageInfoPtr BiometricPipelineBalanced::acquire(const std::string& filename, long task)
	{
		if (task & Verification)
			throw std::exception("cannot acquire only for verification task");

		auto image = push_image_file(filename, task);		
		return image;
	}

	ImageInfoPtr BiometricPipelineBalanced::acquire(const BioContracts::RawImage& raw_image, long task)
	{
		if (task & Verification)
			throw std::exception("cannot acquire only for verification task");

		auto image = push_image(raw_image, task);
		return image;		
	}


	ImageInfoPtr BiometricPipelineBalanced::push_task(TaskInfo task_info)
	{
		auto image_info = std::make_shared<ImageInfo>(task_info.first, task_info.id);

		if (nullptr == image_info)
			return nullptr;

		this->face_find(image_info);

		if (image_info->size() <= 0)
			return nullptr;

		auto task = task_info.second;
		auto image_awaitable = std::make_shared<ImageInfoAwaitable>(image_info, task);
		image_awaitable->init();
		for( auto it = image_awaitable->cbegin(); it != image_awaitable->cend(); ++it)
	   // [&](FaceInfoAwaitablePtr face)
		  {		  	
				FaceInfoAwaitablePtr face = *it;
		  	governor_.wait_available_working_unit();
		  	if (face->has_task(PortraitAnalysis))
		  		asend(*face_analyzer_, face);
		  
		  	if (face->has_task(FaceImageExtraction))
		  		asend(*facial_image_extractor_, face);
		 }
		//);

		image_awaitable->wait_until_empty();

		return image_info;
	}

	ImageInfoPtr BiometricPipelineBalanced::push_image_file(const std::string& filename, long task)
	{
		BioFacialEngine::FaceVacsIOUtils utils;
		auto image = utils.loadFromFile(filename);
		auto result = process_task(image, task, clock());

		return result;
	}

	ImageInfoPtr BiometricPipelineBalanced::push_image(const BioContracts::RawImage& raw_image, long task)
	{
		BioFacialEngine::FaceVacsIOUtils utils;
		std::string pixel_format = raw_image.pixel_format();

		try
		{
			auto image = utils.loadFromBytes(raw_image.bytes(), raw_image.size(), utils.getFormat(pixel_format));
			auto result = process_task(image, task, raw_image.id());
			return result;

		}
		catch ( std::exception& ex)	{
			std::cout << "exception on file format" << std::endl;
			return nullptr;
		}

		

	}

	ImageInfoPtr BiometricPipelineBalanced::process_task(FRsdkTypes::FaceVacsImage image, long task, long id)
	{
		if (image == nullptr)
		{
			std::cout << "bad image file" << std::endl;
			return nullptr;
		}

		TaskInfo workItem;
		workItem.first = image;
		workItem.second = task;
		workItem.id = id;
		return push_task(workItem);
	}


	//TODO refactor verify face
	VerificationResultPtr
	BiometricPipelineBalanced::verify_face(const std::string& object, const std::string& subject, bool fast)
	{
		auto object_task = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_task = object_task | Enrollment;

		BioFacialEngine::VerificationPair images;

		parallel_invoke(
			[&]() {	images.first  = push_image_file(object, object_task); },
			[&]() {	images.second = push_image_file(object, subject_task); }
		);

		if (images.first == nullptr || images.second == nullptr)
		{
			std::cout << "BiometricPipelineBalanced::verify_face bad image data " << std::endl;
			return nullptr;
		}

		auto result = this->verify(images);
		return result;		
	}

	VerificationResultPtr 
	BiometricPipelineBalanced::verify_face( const BioContracts::RawImage& object
		                                    , const BioContracts::RawImage& subject
		                                    , bool fast)
	{
		auto object_task = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_task = object_task | Enrollment;

		BioFacialEngine::VerificationPair images;

		parallel_invoke(
			[&]() {	images.first  = push_image(object , object_task);  },
			[&]() {	images.second = push_image(subject, subject_task); }
		);

		if (images.first == nullptr || images.second == nullptr)
		{
			std::cout << "BiometricPipelineBalanced::verify_face bad image data " << std::endl;
			return nullptr;
		}

		auto result = this->verify(images);
		return result;		
	}

	IdentificationResultPtr
  BiometricPipelineBalanced::identify_face( const std::string& object
		                                      , const std::list<std::string>& subjects
																		      , bool  fast )
	{
		auto object_task  = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_task = object_task | Enrollment;

		BioFacialEngine::IdentificationPair images;
		BioFacialEngine::FaceVacsIOUtils utils;
		parallel_invoke(
			[&]()	{	images.first = push_image_file(object, object_task);},
			[&]()
		  {
		 	parallel_for_each(subjects.begin(), subjects.end(),
				[&](const std::string& item)
			  {				
					auto img = push_image_file(item, subject_task);
					if (img != nullptr)
						images.second.push_back(img);
			  });		
		  }
		);

		if (images.first == nullptr || images.second.size() <= 0)
		{
			std::cout << "BiometricPipelineBalanced::identify_face bad image data " << std::endl;
			return nullptr;
		}

		auto result = this->identify(images);
		return result;
	}

	IdentificationResultPtr
  BiometricPipelineBalanced::identify_face( const RawImage& object
		                                      , const std::list<RawImage>& subjects
																		      , bool  fast )
	{
		auto object_task  = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_task = object_task | Enrollment;

		BioFacialEngine::IdentificationPair images;
		BioFacialEngine::FaceVacsIOUtils utils;
		parallel_invoke(
			[&]()	{	images.first = push_image(object, object_task);},
			[&]()
		  {
		 	parallel_for_each(subjects.begin(), subjects.end(),
				[&](const RawImage& item)
			  {				
					auto img = push_image(item, subject_task);
					if (img != nullptr)
						images.second.push_back(img);
			  });		
		  }
		);

		if (images.first == nullptr || images.second.size() <= 0)
		{
			std::cout << "BiometricPipelineBalanced::identify_face bad image data " << std::endl;
			return nullptr;
		}

		auto result = this->identify(images);
		return result;
	}
	

	void BiometricPipelineBalanced::stop()
	{
		governor_.wait_until_empty();
		done();
	}
}
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
	sitmo::prng_engine BiometricPipelineBalanced::randomizer_;

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
		//queue_sizes.resize(5, 0);
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
		});

		finish_stage_ = std::make_unique<FaceCall>(
			[this](FaceInfoAwaitablePtr pInfo)
		  {
				if (pInfo->done())			
					governor_.free_unit();			
		  });
	}

	ImageInfoPtr BiometricPipelineBalanced::acquire(const std::string& filename, long task)
	{
		auto pair = load_image(filename);
		return acquire(pair, task);
	}

	ImageInfoPtr BiometricPipelineBalanced::acquire(const BioContracts::RawImage& raw_image, long task)
	{
		auto pair = load_image(raw_image);
		return acquire(pair, task);
	}

	ImageInfoPtr BiometricPipelineBalanced::load_image(const std::string& filename) const
	{
		BioFacialEngine::FaceVacsIOUtils utils;
		auto image = utils.loadFromFile(filename);
		return std::make_shared<ImageInfo>(image, abs(static_cast<long>(randomizer_())));
	}

	ImageInfoPtr BiometricPipelineBalanced::load_image(const BioContracts::RawImage& raw_image) const
	{
		BioFacialEngine::FaceVacsIOUtils utils;
		auto image = utils.loadFromBytes( raw_image.bytes()
		                              	, utils.getFormat(raw_image.pixel_format()));
		return std::make_shared<ImageInfo>(image, raw_image.id());
	}

	
	ImageInfoPtr BiometricPipelineBalanced::acquire( ImageInfoPtr image
			                                           , long task)
	{		
		task = task & ~Verification;
		do_task(image, task);
		return image;
	}
	
	void BiometricPipelineBalanced::do_task(ImageInfoPtr image, long task)
	{
		if (nullptr == image)
			return;

		this->face_find(image);

		if (image->size() <= 0)
			throw std::exception("Image does not contain any face");

		auto image_awaitable = std::make_shared<ImageInfoAwaitable>(image, task);
		image_awaitable->init();

		for (auto it = image_awaitable->cbegin(); it != image_awaitable->cend(); ++it)
		{
			FaceInfoAwaitablePtr face = *it;
			governor_.wait_available_working_unit();
			if (face->has_task(PortraitAnalysis))
				asend(*face_analyzer_, face);

			if (face->has_task(FaceImageExtraction))
				asend(*facial_image_extractor_, face);
		}

		image_awaitable->wait_until_empty();
	}

	VerificationResultPtr BiometricPipelineBalanced::verify_face( ImageInfoPtr object
		                                                          , ImageInfoPtr subject
		                                                          , bool fast )
	{
		if (object == nullptr || subject == nullptr)
			return nullptr;

		auto object_task = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_task = object_task | Enrollment;

		BioFacialEngine::VerificationPair images(object, subject);
		parallel_invoke(
			[&]() {	do_task(object , object_task ); },
			[&]() {	do_task(subject, subject_task); }
		);

		return this->verify(images);		
	}
	

	VerificationResultPtr
	BiometricPipelineBalanced::verify_face(const std::string& object, const std::string& subject, bool fast)
	{
		auto object_info  = load_image(object );
		auto subject_info = load_image(subject);
		return verify_face(object_info, subject_info, fast);	
	}
	

	VerificationResultPtr 
	BiometricPipelineBalanced::verify_face( const BioContracts::RawImage& object
		                                    , const BioContracts::RawImage& subject
		                                    , bool fast)
	{
		auto object_info  = load_image(object);
		auto subject_info = load_image(subject);
		return verify_face(object_info, subject_info, fast);
	}

	IdentificationResultPtr
	BiometricPipelineBalanced::identify_face( const std::string& object
			                                    , long population_id
			                                    , bool fast        )
	{
		auto object_task = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto object_info = load_image(object);

		if (object_info == nullptr)
			return nullptr;

		BioFacialEngine::IdentificationPair ident_pair(object_info, population_id);
		do_task(object_info, object_task);

		return this->identify(ident_pair);
	}

	IdentificationResultPtr
	BiometricPipelineBalanced::identify_face( const RawImage& object
			                                    , long population_id
			                                    , bool fast          )
	{

		if (population_id == -1)
			throw std::exception("Population id cannot be -1");

		auto object_task = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto object_info = load_image(object);

		if (object_info == nullptr)
			return nullptr;

		BioFacialEngine::IdentificationPair ident_pair(object_info, population_id);
		do_task(object_info, object_task);

		return this->identify(ident_pair);
	}

	IdentificationResultPtr
  BiometricPipelineBalanced::identify_face( ImageInfoPtr object
		                                      , const std::list<ImageInfoPtr>& subjects
																		      , bool fast )
	{

		if (object == nullptr || subjects.size() <= 0)
			return nullptr;

		auto object_task  = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		auto subject_task = object_task | Enrollment;

		BioFacialEngine::IdentificationPair ident_pair(object);
		
		parallel_invoke(
			[&]()	{	do_task(object, object_task);},
			[&]()
		  {
		 	parallel_for_each(subjects.begin(), subjects.end(),
				[&](ImageInfoPtr item)
			  {		
					ident_pair.add_population_image(item);
					do_task(item, subject_task);				
			  });		
		  }
		);
		
		return this->identify(ident_pair);
	}

	
	IdentificationResultPtr
  BiometricPipelineBalanced::identify_face( const RawImage& object
		                                      , const std::list<RawImage>& subjects
																		      , bool  fast )
	{
		auto object_info  = load_image(object);

		std::list<ImageInfoPtr> subjects_info;
		for ( auto it = subjects.begin(); it != subjects.end(); ++it)
		{
			auto loaded = load_image(*it);
			if (loaded == nullptr)
				subjects_info.push_back(loaded);
		}
		return identify_face(object_info, subjects_info, fast);	
	}

	IdentificationResultPtr
	BiometricPipelineBalanced::identify_face(const std::string& object
		                                      , const std::list<std::string>& subjects
																		      , bool  fast )
	{
		auto object_info  = load_image(object);

		std::list<ImageInfoPtr> subjects_info;
		for ( auto it = subjects.begin(); it != subjects.end(); ++it)
		{
			auto loaded = load_image(*it);
			if (loaded == nullptr)
				subjects_info.push_back(loaded);
		}
		return identify_face(object_info, subjects_info, fast);
	}
	
	long BiometricPipelineBalanced::prepare_identification_population(const std::list<ImageInfoPtr>& subjects)
	{		
		if (subjects.size() <= 0)
			return -1;

		auto task = FACIAL_EXTRACTION | Enrollment;
		parallel_for_each(subjects.begin(), subjects.end(),
		[&](ImageInfoPtr item)
		{
		 	do_task(item, task);
	   });

		return this->create_identification_population(subjects);
	}


	ImageInfoSet 
	BiometricPipelineBalanced::load_identification_population( const std::list<RawImage>& subjects
			                                                     , long& population_id)
	{
		ImageInfoSet subjects_info;
		for (auto it = subjects.begin(); it != subjects.end(); ++it)
		{
			auto loaded = load_image(*it);
			if (loaded != nullptr)
				subjects_info.push_back(loaded);
		}
		population_id = this->prepare_identification_population(subjects_info);
		return subjects_info;
	}

	ImageInfoSet 
	BiometricPipelineBalanced::load_identification_population( const std::list<std::string>& subjects
			                                                     , long& population_id)
	{
		ImageInfoSet subjects_info;
		for (auto it = subjects.begin(); it != subjects.end(); ++it)
		{
			auto loaded = load_image(*it);
			if (loaded != nullptr)
				subjects_info.push_back(loaded);
		}
		population_id = this->prepare_identification_population(subjects_info);
		return subjects_info;
	}
	

	void BiometricPipelineBalanced::stop()
	{
		governor_.wait_until_empty();
		done();
	}
}
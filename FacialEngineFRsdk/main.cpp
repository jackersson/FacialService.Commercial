#include "facial_engine.hpp"
#include <vector>
#include <algorithm>
#include <ppl.h>
#include <agents.h>
#include <tuple>
#include <exception>
#include <stdexcept>
#include <Windows.h>
#include <queue>
#include <time.h>

using namespace ::Concurrency;
using namespace BioFacialEngine;
using namespace BioContracts;

#include <iostream>

typedef std::pair<std::string, long> BioWorkItem;

class PipelineGovernor
{
private:
	struct signal {};

	int m_capacity;
	int m_phase;
	unbounded_buffer<signal> m_completedItems;

public:
	PipelineGovernor(int capacity) : m_phase(0), m_capacity(capacity) {}
		
	void FreePipelineSlot(){
		send(m_completedItems, signal());
	}

	void WaitForAvailablePipelineSlot()	{
		if (m_phase < m_capacity)
			++m_phase;
		else
			receive(m_completedItems);
	}
	
	void WaitForEmptyPipeline()	{
		while (m_phase > 0)
		{
			--m_phase;
			receive(m_completedItems);
		}
	}

private:
	PipelineGovernor(const PipelineGovernor&);
	PipelineGovernor const & operator=(PipelineGovernor const&);
};

typedef std::shared_ptr<FRsdkFaceCharacteristic> FRsdkFaceCharacteristicPtr;

enum BiometricTask : long
{
	  FaceFind             = 1 << 0
	, PortraitAnalysis     = 1 << 1
	, FaceImageExtraction  = 1 << 2
	, IsoComplianceTest    = 1 << 4
	, Enrollment           = 1 << 8
	, Verification         = 1 << 16
};

class FaceInfo
{
public:
	FaceInfo( const FRsdk::Face::Location& faceLocation
		      , const FRsdk::Eyes::Location& eyes
					, const FaceVacsImage          image)
					:	face_ (faceLocation)
					, eyes_ (eyes )
					, image_(image)
					, facial_image_(nullptr)
					, portraitCharacteristics_(nullptr)
					, fir_(new FacialEnrollmentFeedback())
	{
		Utils utils;
		id_ = /*std::abs(utils.getTicks())*/ faceLocation.pos.x();
	}


	int id() const { return id_; }

	const std::string imageId() const	{
		return image_->name();
	}

	bool hasFir() const	{
		return fir_->good();
	}

	bool isVerificationTarget() const
	{
		return hasFir();
	}

	bool hasFacialImage() const {
		return (facial_image_ != nullptr);
	}

	int GetSequence() const { return id_; }		

	const FRsdk::AnnotatedImage annotatedImage() const	{
		return FRsdk::AnnotatedImage(*image_, eyes_.instance());
	}

	const FRsdkFaceCharacteristic& characteristics() const	{
		return *portraitCharacteristics_;
	}

	const FRsdk::Image& facialImage() const	{
		return *facial_image_;
	}

	void setFacialImage(FaceVacsImage token) {
		facial_image_ = token;
	}

	const std::string identificationRecord()	{
		return enrollmentRecord()->bytestring();
	}

	FacialEnrollmentFeedbackPtr enrollmentRecord() const {	
		return fir_;
	}
//private:
	void updatePortraitCharacteristics(FRsdkFaceCharacteristicPtr portrait) {
		portraitCharacteristics_ = portrait;
	}

	void updateIsoCompliance(FRsdkIsoCompliancePtr iso_compliance ) {
		isoCompliance_ = iso_compliance;
	}

private:
	FaceInfo(const FaceInfo& rhs);
	FaceInfo const & operator=(FaceInfo const&);	

private:
	FRsdkFaceW        face_ ;
	FRsdkEyesW        eyes_ ;
	FaceVacsImage     image_;
	FaceVacsImage     facial_image_;

	FacialEnrollmentFeedbackPtr fir_;

	FRsdkFaceCharacteristicPtr portraitCharacteristics_;
	FRsdkIsoCompliancePtr      isoCompliance_;
private:
	int id_;
};
typedef std::shared_ptr<FaceInfo> FaceInfoPtr;

typedef std::pair<FaceInfoPtr, long> FaceInfoTaskItem;


class Veryfier
{
private:
	typedef std::pair<FRsdk::Image, long>  VerificationObject;
	typedef std::pair<FRsdk::FIR  , long>  VerificationSubject;
public:
	Veryfier() : counter_(0) {}

	Veryfier(const std::string& configuration_filename)
	{
		bool is_ok = false;
		try
		{
			FaceVacsConfiguration configuration = new FRsdk::Configuration(configuration_filename);
			is_ok = init(configuration);
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}

		if (!is_ok)
			this->~Veryfier();
	}


	Veryfier(FaceVacsConfiguration configuration)
	{
		if (!init(configuration))
			this->~Veryfier();
	}

	bool init(FaceVacsConfiguration configuration)
	{
		try
		{
			processor_      = new FRsdk::Verification::Processor(*configuration);
			score_mappings_ = new FRsdk::ScoreMappings(*configuration);

			std::shared_ptr<FirBilder> ptr(new FirBilder(configuration));
			fir_builder_ = ptr;
			return true;
		}
		catch (std::exception& e) { std::cout << e.what(); return false; }
	}

	void Add(FaceInfoPtr item)
	{
		if (!item->hasFir() && item->hasFacialImage())
		{
			container_.push_back(item);
			return;
		}

		VerificationObject  verification_object(item->facialImage(), item->id());

		concurrency::parallel_for_each(container_.cbegin(), container_.cend(),
			[&](FaceInfoPtr iter_item)
		  {		  
			  if (item->imageId() == iter_item->imageId())
		  		return;
									  
		  	++counter_;

			VerificationSubject verification_subject(iter_item->enrollmentRecord()->fir(), item->id());
		  
				BioContracts::Match match(verify(verification_object, verification_subject));
				bool match_score = match.match() > 0.5f;
		  
				if (match_score)		  		
					matches_.Add(match);
		  }
		);
	
	}
	
	void clear()	{
		container_.clear();
		matches_.clear();
	}

	BioContracts::Matches matches() const	{
		return matches_;
	}

	int counter() const
	{
		return counter_;
	}

	size_t size() const	{
		return matches_.size();
	}

private:
	BioContracts::Match verify( const VerificationObject & image
		                        , const VerificationSubject& fir     )	
	{
		FRsdk::SampleSet images;
		images.push_back(image.first);
		
		FRsdk::CountedPtr<FacialVerificationFeedback> verification_feedback 
			= new FacialVerificationFeedback(BioContracts::Match(image.second, fir.second, 0.0f));

		FRsdk::Verification::Feedback
			feedback(verification_feedback);

		//std::cout << fir.size() << std::endl;

		processor_->process( images.begin(), images.end()
			                 , fir.first
			                 , 0.01f
											 , feedback);

		return verification_feedback->result();
	}

private:
	std::list<FaceInfoPtr> container_;
	BioContracts::Matches matches_;

	FRsdkVerificationProcessor processor_     ;
	FRsdkScoreMappings         score_mappings_;
	FirBuilderRef              fir_builder_   ;


	int counter_;
};

typedef std::shared_ptr<Veryfier> VeryfierPtr;



class ImageInfo 
{
public:
	ImageInfo(const std::string& filename) : filename_(filename)
	{
		FaceVacsIOUtils utils;
		image_ = utils.loadFromFile(filename);
	}

	FaceVacsImage image() const {
		return image_;
	}

	void addRange(const std::vector<FaceVacsFullFace>& faces)
	{
		parallel_for_each(faces.begin(), faces.end(), 
			[&](const FaceVacsFullFace& face) {
				addFace(face.first, face.second);
		  }
		);
	}

	void addFace( const FRsdk::Face::Location& faceLocation
		          , const FRsdk::Eyes::Location& eyes         )
	{		
		FaceInfoPtr face(new FaceInfo(faceLocation, eyes, image_));		
		faces_.push_back(face);
	}

	const std::string& filename() const { return filename_; }

	size_t size() const	{
		return faces_.size();
	}

public:
	
	const std::list<FaceInfoPtr>::const_iterator cbegin() const{
		return faces_.cbegin();
	}

	const std::list<FaceInfoPtr>::const_iterator cend() const{
		return faces_.cend();
	}
	
private:
	ImageInfo(const ImageInfo& rhs);
	ImageInfo const & operator=(ImageInfo const&);

private:
	std::string filename_;
	std::list<FaceInfoPtr> faces_;
	
	FaceVacsImage image_;
}; 

typedef std::shared_ptr<ImageInfo>    ImageInfoPtr;
typedef std::pair<ImageInfoPtr, long> ImageInfoTaskItem;

class ImageInfoPipelineItem 
{
private:
	struct signal {};

	int m_capacity   ;
	int working_units;
	unbounded_buffer<signal> m_completedItems;

public:
	ImageInfoPipelineItem(ImageInfoPtr ptr) : image_info_(ptr), m_capacity(ptr->size()), working_units(0){}

	void FreePipelineSlot(){
		send(m_completedItems, signal());
	}

	void WaitForAvailablePipelineSlot()	{
		if (working_units < m_capacity)
			++working_units;
		else
			receive(m_completedItems);
	}

	void WaitForObject()	{
		while (working_units > 0)
		{
			--working_units;
			receive(m_completedItems);
		}
	}

private:
	ImageInfoPipelineItem(const ImageInfoPipelineItem&);
	ImageInfoPipelineItem const & operator=(ImageInfoPipelineItem const&);

private:
	ImageInfoPtr image_info_;

};

typedef std::shared_ptr<ImageInfoPipelineItem> ImageInfoPipelineItemPtr;

class PipelineTaskItem
{
public:
	PipelineTaskItem(ImageInfoPipelineItemPtr parent, FaceInfoPtr item, long task) : task_item_(item, task)
		                                                                            , done_query_count_(0)
		                                                                            , finished_tasks_()
																																								, parent_(parent)
	{
		setDone(BiometricTask::FaceFind);
		parent->WaitForAvailablePipelineSlot();
	}
	/*
	PipelineTaskItem(const FaceInfoTaskItem& item) : task_item_(item)
		                                             , done_query_count_(0)
		                                             , finished_tasks_() 
																								 , parent_(item.)
	{
		setDone(BiometricTask::FaceFind);
	}
	*/
	FaceInfoPtr item() const	{
		return task_item_.first;
	}

	bool hasTask(BiometricTask task) const {
		return (task_item_.second & task) == task;
	}

	void setDone(BiometricTask task)	{
		std::lock_guard<std::mutex> lock(g_i_mutex);
		finished_tasks_ |= task;
		
	}
	
	bool done() const {
		bool result = finished_tasks_ == task_item_.second;
		++done_query_count_;

		bool problem = !result && done_query_count_ >= MAX_QUERY_COUNT;
		if (problem)
			std::cout << "not done problem" << std::endl;
		
		if (result)
			parent_->FreePipelineSlot();
		//assert( !result && done_query_count_ >= MAX_QUERY_COUNT );

		return result;
	}

private:
	FaceInfoTaskItem task_item_;	
	ImageInfoPipelineItemPtr parent_;
	long finished_tasks_;
	std::mutex g_i_mutex;
	mutable size_t done_query_count_;

	const size_t MAX_QUERY_COUNT = 3;
};
typedef std::shared_ptr<PipelineTaskItem> PipelineTaskItemPtr;

class IImagePipeline
{
public :
	~IImagePipeline() {}

	virtual ImageInfoPtr LoadBioImage              ( const std::string& filename) = 0;
	virtual void         FaceFind                  ( ImageInfoPtr pInfo   ) = 0;
	virtual void         PortraitCharacteristicFind( FaceInfoPtr  pInfo   ) = 0;
	virtual void         FacialImageExtractor      ( FaceInfoPtr  pInfo   ) = 0;
	virtual void         IsoComplianceTest         ( FaceInfoPtr  pInfo   ) = 0;
	virtual void         Enrollment                ( FaceInfoPtr  pInfo   ) = 0;
	virtual void         Verification              ( FaceInfoPtr  pInfo   ) = 0;
};

typedef std::shared_ptr<IImagePipeline> ImagePipelinePtr;

class FRsdkTasks : public IImagePipeline
{
public:

	FRsdkTasks(const std::string& configuration_filename)
	{
		bool is_ok = false;
		try
		{
			FaceVacsConfiguration configuration = new FRsdk::Configuration(configuration_filename);
			is_ok = init(configuration);
		}
		catch (const FRsdk::FeatureDisabled& e) {
			std::cout << e.what();
		}
		catch (const FRsdk::LicenseSignatureMismatch& e) {
			std::cout << e.what();
		}
		catch (std::exception& e) {
			std::cout << e.what();
		}

		if (!is_ok)
			this->~FRsdkTasks();
	}

	FRsdkTasks(FaceVacsConfiguration configuration)
	{
		if (!init(configuration))
			FRsdkTasks::~FRsdkTasks();
	}

	bool init(FaceVacsConfiguration configuration)
	{
	  try
	  {
	  	parallel_invoke(
				[&]() {
				  FaceVacsAcquisitionPtr ptr(new FacialAcquisition(configuration));
				  acquisition_ = ptr;		
			  },	   
	  	  [&](){enroller_           = new FRsdk::Enrollment::Processor(*configuration);             },
	  	  [&](){

			    VeryfierPtr ver(new Veryfier(configuration));
			    veryfier_ = ver;
		    },
	  	  [&](){	
					FirBuilderRef ptr(new FirBilder(configuration)); fir_builder_ = ptr;
				}
	  
	  	);		
	  	return true;
	  }
	  catch (const FRsdk::FeatureDisabled& e) { std::cout << e.what(); return false; }
	  catch (const FRsdk::LicenseSignatureMismatch& e) { std::cout << e.what(); return false; }
	  catch (std::exception& e) { std::cout << e.what(); return false; }
	}

	ImageInfoPtr LoadBioImage(const std::string& filename)
	{		 
		ImageInfoPtr image(new ImageInfo(filename));	
		return image;
  }

	void FaceFind(ImageInfoPtr pInfo)
	{
		FaceVacsImage image = pInfo->image();

		std::vector<FaceVacsFullFace> faces;
		acquisition_->findFace(image, faces);

		pInfo->addRange(faces);	
	}

	void PortraitCharacteristicFind(FaceInfoPtr pInfo)
	{			
		FaceVacsPortraitCharacteristicsPtr pch = acquisition_->analyze(pInfo->annotatedImage());		
		FRsdkFaceCharacteristicPtr portrait(new FRsdkFaceCharacteristic(*pch));
		pInfo->updatePortraitCharacteristics(portrait);	
	}

	void IsoComplianceTest(FaceInfoPtr pInfo)
	{
		FRsdk::Portrait::Characteristics portrait = pInfo->characteristics().portraitCharacteristics();
		FaceVacsCompliancePtr result = acquisition_->isoComplianceTest(portrait);

		FRsdkIsoCompliancePtr iso(new FRsdkIsoCompliance(*result));
		pInfo->updateIsoCompliance(iso);
	}

	void FacialImageExtractor(FaceInfoPtr pInfo){
		FaceVacsImage result = acquisition_->extractFace(pInfo->annotatedImage());		
		pInfo->setFacialImage(result);
	}

	void Enrollment(FaceInfoPtr pInfo)
	{		
		FRsdk::SampleSet images;

		images.push_back(pInfo->facialImage());

		FRsdk::Enrollment::Feedback
			feedback((pInfo->enrollmentRecord()));

		enroller_->process(images.begin(), images.end(), feedback);		
	}


	void Verification(FaceInfoPtr pInfo)
	{
		veryfier_->Add(pInfo);
	}

private:
	FaceVacsAcquisitionPtr     acquisition_;	
	FRsdkEnrollmentProcessor   enroller_   ;
	VeryfierPtr                veryfier_   ;
	FirBuilderRef              fir_builder_;
private:
	const float MIN_EYE_DISTANCE = 0.1f;
	const float MAX_EYE_DISTANCE = 0.4f;
};

class AgentBase : public agent
{
private:	
	//ISource<bool>& m_cancellationSource;
	//ITarget<ErrorInfo>& m_errorTarget;
	//mutable overwrite_buffer<bool> m_shutdownPending;
	//mutable critical_section m_latestImageLock;
	//ImageInfoPtr m_pLatestImage;

protected:
	int GetPipelineCapacity() const { return 20; }

public:
	AgentBase(ImagePipelinePtr pipeline) : pipeline_(pipeline)
	{
		//send(m_shutdownPending, false);
	}
	virtual int GetQueueSize(int queue) const = 0;

	bool IsCancellationPending() const { return false /*receive(m_shutdownPending) || receive(m_cancellationSource)*/; }
	
	ImageInfoPtr LoadBioImage(const std::string& filename) const
	{
		ImageInfoPtr pInfo = nullptr;
		try
		{
			if (!IsCancellationPending())
				pInfo = pipeline_->LoadBioImage(filename);
		}
		catch (std::exception& e)
		{
			ShutdownOnError(0, filename, e);
		}
	
		return pInfo;
	}

	void FindFace(ImageInfoPtr pInfo) const
	{
		try
		{
			if (!IsCancellationPending() && (nullptr != pInfo))			
				pipeline_->FaceFind(pInfo);
			
			std::cout << "FaceFind" << std::endl;
		}
		catch (std::exception& e)	{
			ShutdownOnError(0, pInfo->filename(), e);
		}		
	}
	
	void IsoComplianceTest(PipelineTaskItemPtr pInfo) const
	{
		try
		{
			FaceInfoPtr face = pInfo->item();
			if (!IsCancellationPending() && (nullptr != face))
			{
				pipeline_->IsoComplianceTest(face);
				pInfo->setDone(BiometricTask::IsoComplianceTest);

				std::cout << "IsoComplianceTest" << std::endl;
			}
		}
		catch (std::exception& e)	{
			ShutdownOnError(0, "eyes find", e);
		}
	}
	

	void PortraitCharacteristicFind(PipelineTaskItemPtr pInfo) const
	{
		try
		{
			FaceInfoPtr face = pInfo->item();
			if (!IsCancellationPending() && (nullptr != face))
			{
				pipeline_->PortraitCharacteristicFind(face);
				pInfo->setDone(BiometricTask::PortraitAnalysis);

				std::cout << "PortraitAnalysis" << std::endl;
			}
		}
		catch (std::exception& e)	{
			ShutdownOnError(0, "portrait characteristic find", e);
		}
	}

	void Extract(PipelineTaskItemPtr pInfo) const
	{
		try
		{
			FaceInfoPtr face = pInfo->item();
			if (!IsCancellationPending() && (nullptr != face))
			{
				pipeline_->FacialImageExtractor(face);
				pInfo->setDone(BiometricTask::FaceImageExtraction);

				std::cout << "FaceImageExtraction" << std::endl;
			}
		}
		catch (std::exception& e)	{
			ShutdownOnError(0, "portrait characteristic find", e);
		}
	}

	void Enrollment(PipelineTaskItemPtr pInfo) const
	{
		try
		{
			FaceInfoPtr face = pInfo->item();
			if (!IsCancellationPending() && (nullptr != face))
			{
				pipeline_->Enrollment(face);
				pInfo->setDone(BiometricTask::Enrollment);
			}
		}
		catch (std::exception& e)	{
			ShutdownOnError(0, "portrait characteristic find", e);
		}
	}

	void Verification(FaceInfoPtr pInfo) const
	{
		try
		{
			if (!IsCancellationPending() && (nullptr != pInfo))
				pipeline_->Verification(pInfo);
		}
		catch (std::exception& e)	{
			ShutdownOnError(0, "portrait characteristic find", e);
		}
	}

	void Finish(FaceInfoPtr pInfo) const
	{
		try
		{
			std::cout << "finish" << std::endl;
		}
		catch (std::exception& e)	{
			ShutdownOnError(0, "portrait characteristic find", e);
		}
	}
		

private:
	void ShutdownOnError(long phase, const ImageInfoPtr pInfo, const std::exception& const e) const
	{
		ShutdownOnError(phase, pInfo->filename(), e);
	}

	void ShutdownOnError(long phase, const std::string& filePath, const std::exception& const e) const
	{
		std::string message = e.what();		
		SendError(phase, filePath, message);
	}		

	void SendError(long phase, const std::string& filePath, const std::string message) const
	{		
		//send(m_shutdownPending, true);
		//send(m_errorTarget    , ErrorInfo(phase, filePath, message));		
	}

	private:
		ImagePipelinePtr pipeline_;
};


class ImageAgentPipelineBalanced : public AgentBase
{
private:
	std::unique_ptr<call<ImageInfoTaskItem>>    m_face_finder_           ;
	std::unique_ptr<call<PipelineTaskItemPtr>>  m_facial_image_extractor_;
	std::unique_ptr<call<PipelineTaskItemPtr>>  m_face_analyzer_         ;
	std::unique_ptr<call<PipelineTaskItemPtr>>  m_iso_compliance_test_   ;
	std::unique_ptr<call<PipelineTaskItemPtr>>  m_enrollment_processor_  ;
	std::unique_ptr<call<PipelineTaskItemPtr>>  m_verification_processor_;
	std::unique_ptr<call<PipelineTaskItemPtr>>  m_finish_;
			
	PipelineGovernor m_governor;
	
	std::vector<long> m_queueSizes;

	static const  unsigned int MAX_PIPELINE_SLOT_COUNT = 25;
public:
	ImageAgentPipelineBalanced(ImagePipelinePtr pipeline) : AgentBase(pipeline)
		                                                    , m_governor(MAX_PIPELINE_SLOT_COUNT)
		                                                    , m_face_finder_           (nullptr)
		                                                    , m_facial_image_extractor_(nullptr)
		                                                    , m_face_analyzer_         (nullptr)
		                                                    , m_iso_compliance_test_   (nullptr)
		                                                    , m_enrollment_processor_  (nullptr)
		                                                    , m_finish_(nullptr)		
	{
	
		m_queueSizes.resize(5, 0);
		Initialize(); 
	}
	
	void Initialize()
	{  				 
		m_facial_image_extractor_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
		  [this](PipelineTaskItemPtr pInfo)
      {
			 this->Extract(pInfo);						
			 if (pInfo->hasTask(BiometricTask::Enrollment))				 
				 asend(*m_enrollment_processor_, pInfo);
			 else 
				 asend(*m_finish_, pInfo);
       }          
    ));		

		m_face_analyzer_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
		  [this](PipelineTaskItemPtr pInfo)
		  {
		 	 this->PortraitCharacteristicFind(pInfo);		  
		 	 if (pInfo->hasTask(BiometricTask::IsoComplianceTest))
		 		 asend(*m_iso_compliance_test_, pInfo);
		 	 else
		 		 asend(*m_finish_, pInfo);
		  }
		));
		 
	  m_enrollment_processor_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
	    [this](PipelineTaskItemPtr pInfo)
		  {
		    this->Enrollment(pInfo);
		    if (pInfo->hasTask(BiometricTask::Verification))				 
		   	  asend(*m_verification_processor_, pInfo);				 
			  else
			    asend(*m_finish_, pInfo);
		  }
	  ));

		m_verification_processor_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
			[this](PipelineTaskItemPtr pInfo)
		  {
		    this->Verification(pInfo->item());			
			  asend(*m_finish_, pInfo);
		  }
		));		

	  m_iso_compliance_test_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
			[this](PipelineTaskItemPtr pInfo)
		  {
			  this->IsoComplianceTest(pInfo);
			  asend(*m_finish_, pInfo);
		  }
	  ));
		 
	   m_finish_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
			[this](PipelineTaskItemPtr pInfo)
      {               
			  if (pInfo->done())			   				
				  m_governor.FreePipelineSlot();			   						
      }			
    ));			
  }

	void run()
	{}

	 //void push(const std::vector<BioWorkItem>& filenames)
	// {
		 //parallel_for_each(filenames.cbegin(), filenames.cend(), 
	   //[&](BioWorkItem work_item) {
			// push(work_item);
		// });		
	// }

	ImageInfoPtr push(const BioWorkItem& work_item)
	{
		ImageInfoPtr pInfo = this->LoadBioImage(work_item.first);
		 		 
		if (nullptr == pInfo)
			return nullptr;



	  ImageInfoTaskItem info(pInfo, work_item.second);
		this->FindFace(pInfo);

		ImageInfoPipelineItemPtr ptr(new ImageInfoPipelineItem(pInfo));

		long task_configuration = work_item.second;
		parallel_for_each(pInfo->cbegin(), pInfo->cend(),
		  [&](FaceInfoPtr face)
		  {
				PipelineTaskItemPtr item(new PipelineTaskItem(ptr, face, task_configuration));
		  
		  	m_governor.WaitForAvailablePipelineSlot();
		  	if (item->hasTask(BiometricTask::PortraitAnalysis))
		  	 asend(*m_face_analyzer_, item);
		  
		  	if (item->hasTask(BiometricTask::FaceImageExtraction))
		  	 asend(*m_facial_image_extractor_, item);		  
	    });				 

		ptr->WaitForObject();

		//m_governor.WaitForEmptyPipeline();

		return pInfo; 
	}

	 void acquire(const std::string& object, long configuration = FAST_PORTRAIT_ANALYSIS) 
	 {
		 if (configuration &= BiometricTask::Verification)
			 throw new std::exception("Veryfication");

		 BioWorkItem workItem(object, configuration);
		 push(workItem);
	 }


	void verify(const std::string object, const std::string subject, bool fast = false)
	{
		long object_configuration  = fast ? FACIAL_EXTRACTION : FULL_PORTRAIT_ANALYSIS;
		long subject_configuration = object_configuration | BiometricTask::Enrollment ;

		std::vector<BioWorkItem> items;
		items.push_back(BioWorkItem(object , object_configuration ));
		items.push_back(BioWorkItem(subject, subject_configuration));

		//push(items);
  }

  void identify(const std::string& object, const std::vector<std::string>& subjects) {

  }

  void stop()
  {
    m_governor.WaitForEmptyPipeline();
	  done();
  }

  int GetQueueSize(int queue) const { return m_queueSizes[queue]; }

private:
  ImagePipelinePtr pipeline_;
  
	static const long FACIAL_EXTRACTION      = FaceFind | FaceImageExtraction;
  static const long FAST_PORTRAIT_ANALYSIS = FaceFind | PortraitAnalysis;
  static const long ISO_COMPLIANCE_TEST    = FAST_PORTRAIT_ANALYSIS | BiometricTask::IsoComplianceTest;
  static const long FULL_PORTRAIT_ANALYSIS = ISO_COMPLIANCE_TEST | FaceImageExtraction;
  
	static const long FAST_ENROLLMENT        = FACIAL_EXTRACTION | BiometricTask::Enrollment;
  static const long FULL_ENROLLMENT        = FULL_PORTRAIT_ANALYSIS | BiometricTask::Enrollment;
};

int main(int argc, char** argv)
{	

	std::string cfg_path = "C:\\FVSDK_8_9_5\\etc\\frsdk.cfg";
	std::shared_ptr<FRsdkTasks> task(new FRsdkTasks(cfg_path));

	long acquire = FaceFind | PortraitAnalysis | FaceImageExtraction | IsoComplianceTest | Enrollment;
	long acquire2 = FaceFind | PortraitAnalysis | FaceImageExtraction;
	std::vector<BioWorkItem> filenames;
	BioWorkItem test("C:\\Users\\jacke\\Desktop\\3423.jpg", acquire);
	BioWorkItem test2("C:\\Users\\jacke\\Desktop\\shwarc.jpg", acquire);
	filenames.push_back(BioWorkItem(test));
	//filenames.push_back(BioWorkItem("C:\\Users\\jacke\\Desktop\\shwarc.jpg" , acquire));
	//filenames.push_back(BioWorkItem("C:\\Users\\jacke\\Desktop\\shwarc.jpg", acquire));
	
	/*
	unsigned int start = clock();

	parallel_for_each(filenames.cbegin(), filenames.cend(),
		[&](BioWorkItem item) {
		ImageInfoPtr ptr = task->LoadBioImage(item.first);
		task->FaceFind(ptr);

		concurrency::parallel_for_each(ptr->cbegin(), ptr->cend(),
			[&](FaceInfoPtr face)
		{
			task->FacialImageExtractor(face);
			task->PortraitCharacteristicFind(face);
			task->IsoComplianceTest(face);
			task->Enrollment(face);
		});
	});

	std::cout << "no" << clock() - start << std::endl;
	*/
	

	ImageAgentPipelineBalanced pipeline(task);

	pipeline.run();

	unsigned int start = clock();
	ImageInfoPtr ptr1 = nullptr;
	ImageInfoPtr ptr2 = nullptr;
	parallel_invoke(
		[&]() { ptr1 = pipeline.push(test); },
		[&]() { ptr2 = pipeline.push(test2); }
		
	);


	for (auto it = ptr1->cbegin(); it != ptr1->cend(); ++it)
	{
		std::cout << "has fir = " << (*it)->id() << " " << (*it)->hasFir() << std::endl;
	}

	for (auto it = ptr2->cbegin(); it != ptr2->cend(); ++it)
	{
		std::cout << "has fir = " <<  (*it)->id() << " " << (*it)->hasFir() << std::endl;
	}

	std::cout << "pipeline = " << clock() - start << std::endl;
	std::cin.get();
	std::cout << std::endl;	


	return 0;
}




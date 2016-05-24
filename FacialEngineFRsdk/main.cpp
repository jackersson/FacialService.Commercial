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

#include <iostream>

class IPosition
{
public :
	virtual float x() const = 0;
	virtual float y() const = 0;	
};

class IFace
{
public:
	virtual float confidence()    const = 0;
	virtual float width()         const = 0;
	virtual float rotationAngle() const = 0;	 

	virtual const IPosition& position()  const = 0;
};

class FRsdkPositionW : public IPosition
{
public:
	FRsdkPositionW(const FRsdk::Position& position) : position_(position) {}

	float x() const {
		return position_.x();
	}

	float y() const {
		return position_.y();
	}

	const FRsdk::Position& instance() const {
		return position_;
	}

private:
	FRsdk::Position position_;;
};

class FRsdkFaceW : public IFace
{
public:
	FRsdkFaceW(const FRsdk::Face::Location& location) : face_(location) {
		std::cout << "face : " << location.pos.x() << " " << location.pos.y() << std::endl;
	}

	float confidence() const {
		return face_.confidence;
	}

	float width()   const {
		return face_.width;
	}

	float rotationAngle() const {
		return face_.rotationAngle;
	}

	const FRsdkPositionW& position() const {
		return face_.pos;
	}

	const FRsdk::Face::Location& instance() const {
		return face_;
	}

private:
	FRsdk::Face::Location face_;
};

class IEye
{
public: 
	virtual const IPosition& position() const = 0;

	virtual float confidence() const = 0;
};

class FRsdkEye : public IEye
{
public:
	FRsdkEye(const FRsdk::Position& pos, float confidence) : position_(pos), confidence_(confidence) {
		std::cout << "eyes : " << pos.x() << " " << pos.y() << std::endl;
	}

	const FRsdkPositionW& position() const	{
		return position_;
	}

	float confidence() const {
		return confidence_;
	}
private:
	float confidence_;
	FRsdkPositionW position_;
};

class IEyes
{
public:
	virtual const IEye& left() const = 0;
	virtual const IEye& right() const = 0;
};


class FRsdkEyesW : public IEyes
{
public:
	FRsdkEyesW(  const FRsdk::Eyes::Location& location) 
		         : eyes_  (location)
		         , eyes_w_( FRsdkEye(location.first , location.firstConfidence )
		         , FRsdkEye(location.second, location.secondConfidence))
	{}

	const IEye& left() const {
		return eyes_w_.first;
	}

	const IEye& right() const {
		return eyes_w_.second;
	}
		
	const FRsdk::Eyes::Location& instance() const {
		return eyes_;
	}
private:
	FRsdk::Eyes::Location eyes_;
	std::pair<FRsdkEye, FRsdkEye> eyes_w_;
};

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
		id_ = std::abs(utils.getTicks());
	}


	int id() const { return id_; }

	const std::string& imageId() const	{
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

class PipelineTaskItem
{
public:
	PipelineTaskItem(FaceInfoPtr item, long task) : task_item_(item, task)
		                                            , done_query_count_(0)
		                                            , finished_tasks_()
	{
		setDone(BiometricTask::FaceFind);
	}

	PipelineTaskItem(const FaceInfoTaskItem& item) : task_item_(item)
		                                             , done_query_count_(0)
		                                             , finished_tasks_() 
	{
		setDone(BiometricTask::FaceFind);
	}

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
		
		//assert( !result && done_query_count_ >= MAX_QUERY_COUNT );

		return result;
	}

private:
	FaceInfoTaskItem task_item_;	

	long finished_tasks_;
	std::mutex g_i_mutex;
	mutable size_t done_query_count_;

	const size_t MAX_QUERY_COUNT = 3;
};
typedef std::shared_ptr<PipelineTaskItem> PipelineTaskItemPtr;

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
	  
	    [&](){face_finder_        = new FRsdk::Face::Finder(*configuration);											},
	    [&](){eyes_finder_        = new FRsdk::Eyes::Finder(*configuration);											},
	  	[&](){portrait_analyzer_  = new FRsdk::Portrait::Analyzer(*configuration);								},
	    [&](){iso_19794_test_     = new FRsdk::ISO_19794_5::FullFrontal::Test(*configuration);		},
	    [&](){token_face_creator_ = new FRsdk::ISO_19794_5::TokenFace::Creator(*configuration);		},
	  	[&](){enroller_           = new FRsdk::Enrollment::Processor(*configuration);             },
	  	[&](){

			VeryfierPtr ver(new Veryfier(configuration));
			veryfier_ = ver;
		},
	  	[&](){	FirBuilderRef ptr(new FirBilder(configuration)); fir_builder_ = ptr; }
	  
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
		std::cout << "loaded face " << filename << std::endl;
		return image;
  }

	void FaceFind(ImageInfoPtr pInfo)
	{
		FaceVacsImage image = pInfo->image();
		FRsdk::Face::LocationSet faceLocations =
			face_finder_->find(*image, MIN_EYE_DISTANCE, MAX_EYE_DISTANCE);

		if (faceLocations.size() < 1)
		{
			std::cout << "any face not found";
			return;
		}
	
		concurrency::parallel_for_each( faceLocations.cbegin(), faceLocations.cend(),
		                              	[&](FRsdk::Face::Location face)
		{
			FRsdk::Eyes::LocationSet eyesLocations = eyes_finder_->find(*image, face);
			if (eyesLocations.size() > 0)						
				pInfo->addFace(face, eyesLocations.front());			
		});	
	}

	void PortraitCharacteristicFind(FaceInfoPtr pInfo)
	{			
		FRsdkFaceCharacteristicPtr portrait(new FRsdkFaceCharacteristic(portrait_analyzer_->analyze(pInfo->annotatedImage())));
		pInfo->updatePortraitCharacteristics(portrait);	
	}

	void IsoComplianceTest(FaceInfoPtr pInfo)
	{
		FRsdk::Portrait::Characteristics portrait = pInfo->characteristics().portraitCharacteristics();
		FRsdkIsoCompliancePtr iso(new FRsdkIsoCompliance(iso_19794_test_->assess(portrait)));
		pInfo->updateIsoCompliance(iso);
	}

	void FacialImageExtractor(FaceInfoPtr pInfo){
		FaceVacsImage image = new FRsdk::Image(token_face_creator_->extract(pInfo->annotatedImage()).first);
		pInfo->setFacialImage(image);
	}

	void Enrollment(FaceInfoPtr pInfo)
	{		
		FRsdk::SampleSet images;

		images.push_back(pInfo->facialImage());

		FRsdk::Enrollment::Feedback
			feedback((pInfo->enrollmentRecord()));

		enroller_->process(images.begin(), images.end(), feedback);
		std::cout << "enrolled " << pInfo->id() << std::endl;
	}


	void Verification(FaceInfoPtr pInfo)
	{
		veryfier_->Add(pInfo);
	}

private:
	FaceVacsFaceFinder         face_finder_       ;
	FaceVacsEyesFinder         eyes_finder_       ;
	FaceVacsTfcreator          token_face_creator_;
	FaceVacsPortraitAnalyzer   portrait_analyzer_ ;
	FaceVacsIso19794Test       iso_19794_test_    ;
	FRsdkEnrollmentProcessor   enroller_          ;
	VeryfierPtr                veryfier_          ;
	FirBuilderRef              fir_builder_       ;
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
public:
	ImageAgentPipelineBalanced(ImagePipelinePtr pipeline) : AgentBase(pipeline)
		                                                    , m_governor(GetPipelineCapacity() * 3)
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
	m_face_finder_ = std::unique_ptr<call<ImageInfoTaskItem>>(new call<ImageInfoTaskItem>(
			 [this](ImageInfoTaskItem pInfo)
        {
				  ImageInfoPtr image = pInfo.first;
				  long task          = pInfo.second;

				  this->FindFace(image);

				parallel_for_each(image->cbegin(), image->cend(),
				  [&](FaceInfoPtr face)
				  {
				   
					 PipelineTaskItemPtr item(new PipelineTaskItem(face, task));
				  
				   if (item->hasTask(BiometricTask::PortraitAnalysis))
				  	 asend(*m_face_analyzer_, item);
				  
					 if (item->hasTask(BiometricTask::FaceImageExtraction))
				  	 asend(*m_facial_image_extractor_, item);
				  	 
				  });					
        } 
     ));		 
		 
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

		
		 
		 m_enrollment_processor_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
		 [this](PipelineTaskItemPtr pInfo)
		 {
			  this->Enrollment(pInfo);
			 if (pInfo->hasTask(BiometricTask::Verification))
				 {
					 asend(*m_verification_processor_, pInfo);
				 }
			   else
			  	 asend(*m_finish_, pInfo);
		 }
	 ));

		 m_verification_processor_ = std::unique_ptr<call<PipelineTaskItemPtr>>(new call<PipelineTaskItemPtr>(
			 [this](PipelineTaskItemPtr pInfo)
		 {
			 this->Enrollment(pInfo);
			 if (pInfo->hasTask(BiometricTask::Verification))
			 {
				 //asend(*m_verification, pInfo);
			 }
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

	 void push(const std::vector<BioWorkItem>& filenames)
	 {
		 parallel_for_each(filenames.cbegin(), filenames.cend(), 
	   [&](BioWorkItem work_item) {
			 push(work_item);
		 });		 
	 }

	 void push(const BioWorkItem& work_item) {
		 ImageInfoPtr pInfo = this->LoadBioImage(work_item.first);
		 if (nullptr != pInfo)
		 {
			 ImageInfoTaskItem info(pInfo, work_item.second);
			 m_governor.WaitForAvailablePipelineSlot();
			 asend(*m_face_finder_, info);
		 }

		// return pInfo; 
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

		push(items);
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
	/*
	long acquiredd = FaceFind | PortraitAnalysis | FaceImageExtraction | IsoComplianceTest;
	long t = 0;

	t |= BiometricTask::FaceFind;
	std::cout << t << " " << BiometricTask::FaceFind << std::endl;

	t |= BiometricTask::FaceImageExtraction;
	std::cout << t << " " << BiometricTask::FaceImageExtraction << std::endl;

	t |= BiometricTask::PortraitAnalysis;
	std::cout << t << " " << BiometricTask::PortraitAnalysis << std::endl;

	t |= BiometricTask::IsoComplianceTest;
	std::cout << t << " " << BiometricTask::IsoComplianceTest << " " << acquiredd << std::endl;

	std::cin.get();
	return 0;
	*/

	std::string cfg_path = "C:\\FVSDK_8_9_5\\etc\\frsdk.cfg";
	std::shared_ptr<FRsdkTasks> task(new FRsdkTasks(cfg_path));

	long acquire = FaceFind | PortraitAnalysis /*| FaceImageExtraction*/ | IsoComplianceTest;//| Enrollment;
	long acquire2 = FaceFind | PortraitAnalysis | FaceImageExtraction;
	std::vector<BioWorkItem> filenames;
	filenames.push_back(BioWorkItem("C:\\Users\\jacke\\Desktop\\3423.jpg" , acquire));
	filenames.push_back(BioWorkItem("C:\\Users\\jacke\\Desktop\\3423.jpg" , acquire));
	//filenames.push_back(BioWorkItem("C:\\Users\\jacke\\Desktop\\shwarc.jpg", acquire));
	
	unsigned int start = clock();

	parallel_for_each(filenames.cbegin(), filenames.cend(),
		[&](BioWorkItem item) {
		ImageInfoPtr ptr = task->LoadBioImage(item.first);
		task->FaceFind(ptr);

		concurrency::parallel_for_each(ptr->cbegin(), ptr->cend(),
			[&](FaceInfoPtr face)
		{
			task->PortraitCharacteristicFind(face);
			task->IsoComplianceTest(face);
		});
	});

	std::cout << "no" << clock() - start << std::endl;
	
	

	ImageAgentPipelineBalanced pipeline(task);

	pipeline.run();

	 start = clock();
	pipeline.push(filenames);
	pipeline.stop();
	std::cout << "pipeline = " << clock() - start << std::endl;
	
	std::cin.get();
	std::cout << std::endl;
	/*
	long acquire2 = FaceFind | PortraitAnalysis ;
	filenames.clear();
	filenames.push_back(BioWorkItem("C:\\Users\\jacke\\Desktop\\shwarc.jpg", acquire2));
	pipeline.push(filenames);
	pipeline.stop();
	*/

	//std::string cfg_path = "C:\\FVSDK_8_9_5\\etc\\frsdk.cfg";
	//FacialEngine engine(cfg_path);

	//std::string filename  = "C:\\Users\\jacke\\Desktop\\3423.jpg";
	//std::string filename2 = "C:\\Users\\jacke\\Desktop\\shwarc.jpg";

	//std::string file1( readFileBytes(filename));
	//std::string file2( readFileBytes(filename2));

	//RawImage raw1 = readFileBytes(filename);//, file1.size());
	//RawImage raw2 = readFileBytes(filename2);

	//engine.acquire(filename);

	//std::cin.get();


	return 0;
}



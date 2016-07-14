#ifndef BiometricPipelineBalanced_INCLUDED
#define BiometricPipelineBalanced_INCLUDED

#include "pipeline/pipeline_agent.hpp"
#include "common/verification_result.hpp"
#include "common/raw_image.hpp"

#include <queue>
#include <common/identification_result.hpp>

namespace Pipeline
{

	enum BimetricMultiTask
	{
		  FACIAL_EXTRACTION      = FaceFind | FaceImageExtraction
		, FAST_PORTRAIT_ANALYSIS = FaceFind | PortraitAnalysis
		, ISO_COMPLIANCE_TEST    = FAST_PORTRAIT_ANALYSIS | IsoComplianceTest
		, FULL_PORTRAIT_ANALYSIS = ISO_COMPLIANCE_TEST |  FaceImageExtraction
		, FAST_ENROLLMENT        = FACIAL_EXTRACTION | Enrollment
		, FULL_ENROLLMENT        = FULL_PORTRAIT_ANALYSIS | Enrollment
	};

	struct CompareAwaitableItem
	{
		bool operator()(const FaceInfoAwaitablePtr lhs, const FaceInfoAwaitablePtr rhs) const
		{
			return (lhs->start_time() > rhs->start_time());
		}
	};

	
	typedef std::list<FRsdkEntities::ImageInfoPtr> ImageInfoSet;

	class BiometricPipelineBalanced : BiometricPipelineAgent
	{		
		struct TaskInfo
		{
			TaskInfo( long task, FRsdkEntities::ImageInfoPtr image )
				      : pair_(task, image) {}

			FRsdkTypes::FaceVacsImage image( ) const	{
				return pair_.second->image();
			}

			long image_id() const {
				return pair_.second->id();
			}

			long task() const {
				return pair_.first;
			}			
		
		private:
			std::pair<long, FRsdkEntities::ImageInfoPtr> pair_;
		};
		//typedef std::pair<FRsdkTypes::FaceVacsImage, long> TaskInfo;

		typedef std::unique_ptr<Concurrency::call<FaceInfoAwaitablePtr >> FaceInfoAwaitableCallPtr  ;
		typedef std::unique_ptr<Concurrency::call<ImageInfoAwaitablePtr>> ImageInfoAwaitableCallPtr ;
		typedef Concurrency::unbounded_buffer<FaceInfoAwaitablePtr>   FaceInfoAwaitableBuffer   ;
	
	public:
		explicit BiometricPipelineBalanced(IBiometricProcessorPtr pipeline);			                                

		void initialize();
		
		void run() override {}

		FRsdkEntities::ImageInfoPtr acquire( const std::string& object
			                                 , long configuration = FAST_PORTRAIT_ANALYSIS);

		FRsdkEntities::ImageInfoPtr acquire( const BioContracts::RawImage& raw_image
			                                 , long task = FAST_PORTRAIT_ANALYSIS);
	

		BioContracts::VerificationResultPtr verify_face( const std::string& object
			                                             , const std::string& subject
			                                             , bool fast = false);

		BioContracts::VerificationResultPtr verify_face( const BioContracts::RawImage& object
			                                             , const BioContracts::RawImage& subject
			                                             , bool fast = false);
		

		BioContracts::IdentificationResultPtr identify_face( const std::string& object
			                                                 , const std::list<std::string>& subjects
			                                                 , bool fast = false);

		BioContracts::IdentificationResultPtr
    identify_face( const std::string& object
		             , long population_id
							   , bool  fast = false );

		BioContracts::IdentificationResultPtr
    identify_face( const BioContracts::RawImage& object
		             , const std::list<BioContracts::RawImage>& subjects
							   , bool  fast = false );

		BioContracts::IdentificationResultPtr
    identify_face( const BioContracts::RawImage& object
		             , long population_id
							   , bool  fast = false );

		ImageInfoSet load_identification_population( const std::list<BioContracts::RawImage>& subjects
			                                                , long& population_id);
		ImageInfoSet load_identification_population( const std::list<std::string>& subjects
			                                                , long& population_id);
		

		void stop();		

		int GetQueueSize(int queue) const { return queue_sizes[queue]; }

	private:

		BioContracts::VerificationResultPtr verify_face( FRsdkEntities::ImageInfoPtr object
		                                               , FRsdkEntities::ImageInfoPtr subject
		                                               , bool fast = false);

		BioContracts::IdentificationResultPtr identify_face( FRsdkEntities::ImageInfoPtr object
		                                                   , const std::list<FRsdkEntities::ImageInfoPtr>& subjects
		                                                   , bool fast = false);

		FRsdkEntities::ImageInfoPtr acquire(FRsdkEntities::ImageInfoPtr
			                                   , long task = FAST_PORTRAIT_ANALYSIS);

		FRsdkEntities::ImageInfoPtr load_image(const std::string& filename) const;
		FRsdkEntities::ImageInfoPtr load_image(const BioContracts::RawImage& raw_image) const;

		long prepare_identification_population(const std::list<FRsdkEntities::ImageInfoPtr>& subjects);

		void do_task(FRsdkEntities::ImageInfoPtr image, long task);
	

		PipelineGovernor governor_;

		ImageInfoAwaitableCallPtr face_finder_;
		FaceInfoAwaitableCallPtr  facial_image_extractor_;
		FaceInfoAwaitableCallPtr  face_analyzer_;
		FaceInfoAwaitableCallPtr  iso_compliance_test_;

		std::vector<FaceInfoAwaitableCallPtr>  enrollment_processors_;

		FaceInfoAwaitableCallPtr  verification_processor_;
		FaceInfoAwaitableCallPtr  finish_stage_;

		FaceInfoAwaitableCallPtr enrollment_multiplexer_;

		std::priority_queue< FaceInfoAwaitableCallPtr, std::vector<FaceInfoAwaitablePtr>
			                 , CompareAwaitableItem> enrollment_multiplex_queue_;

		FaceInfoAwaitableBuffer enrollment_multiplex_buffer_;
		
		int enrollment_next_filter_id_;
		std::vector<long> queue_sizes;

		static const  unsigned int MAX_PIPELINE_SLOT_COUNT = 25;
		static const  unsigned int MAX_ENROLMENT_BRANCHES_COUNT = 3;

		static sitmo::prng_engine randomizer_;

		
	};

	
}
#endif

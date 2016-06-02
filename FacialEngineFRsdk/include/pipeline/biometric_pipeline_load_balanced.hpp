#ifndef BiometricPipelineBalanced_INCLUDED
#define BiometricPipelineBalanced_INCLUDED

#include "pipeline/pipeline_agent.hpp"
#include "common/verification_result.hpp"

#include <queue>
#include <common/identification_result.hpp>

namespace BioContracts{
	class RawImage;
}

namespace Pipeline
{

	enum BimetricMultiTask
	{
		  FACIAL_EXTRACTION      = FaceFind | FaceImageExtraction
		, FAST_PORTRAIT_ANALYSIS = FaceFind | PortraitAnalysis
		, ISO_COMPLIANCE_TEST    = FAST_PORTRAIT_ANALYSIS | IsoComplianceTest
		, FULL_PORTRAIT_ANALYSIS = ISO_COMPLIANCE_TEST | FaceImageExtraction
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

	class BiometricPipelineBalanced : private BiometricPipelineAgent
	{
		typedef std::pair<FRsdkTypes::FaceVacsImage, long> TaskInfo;

		typedef std::unique_ptr<Concurrency::call<FaceInfoAwaitablePtr >> FaceInfoAwaitableCallPtr  ;
		typedef std::unique_ptr<Concurrency::call<ImageInfoAwaitablePtr>> ImageInfoAwaitableCallPtr ;
		typedef Concurrency::unbounded_buffer<FaceInfoAwaitablePtr>   FaceInfoAwaitableBuffer   ;

	
	public:
		explicit BiometricPipelineBalanced( IBiometricProcessorPtr pipeline) 
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

		void initialize();
		
		void run() override {}

		FRsdkEntities::ImageInfoPtr push(TaskInfo work_item);


		FRsdkEntities::ImageInfoPtr acquire(const std::string& object, long configuration = FAST_PORTRAIT_ANALYSIS);

		FRsdkEntities::ImageInfoPtr acquire(const BioContracts::RawImage& raw_image, long task = FAST_PORTRAIT_ANALYSIS);
	

		BioContracts::VerificationResultPtr verify_face( const std::string& object
			                                               , const std::string& subject
			                                               , bool fast = false);
		

		BioContracts::IdentificationResultPtr identify_face( const std::string& object
			                                                 , const std::vector<std::string>& subjects
			                                                 , bool fast = false);

		void stop();
		

		int GetQueueSize(int queue) const { return queue_sizes[queue]; }

	private:
		PipelineGovernor governor_;

		ImageInfoAwaitableCallPtr face_finder_;
		FaceInfoAwaitableCallPtr  facial_image_extractor_;
		FaceInfoAwaitableCallPtr  face_analyzer_;
		FaceInfoAwaitableCallPtr  iso_compliance_test_;

		std::vector<FaceInfoAwaitableCallPtr>  enrollment_processors_;

		FaceInfoAwaitableCallPtr  verification_processor_;
		FaceInfoAwaitableCallPtr  finish_stage_;

		FaceInfoAwaitableCallPtr enrollment_multiplexer_;
		std::priority_queue<FaceInfoAwaitableCallPtr
			, std::vector<FaceInfoAwaitablePtr>
			, CompareAwaitableItem> enrollment_multiplex_queue_;
		FaceInfoAwaitableBuffer enrollment_multiplex_buffer_;
		
		int enrollment_next_filter_id_;
		std::vector<long> queue_sizes;

		static const  unsigned int MAX_PIPELINE_SLOT_COUNT = 25;
		static const  unsigned int MAX_ENROLMENT_BRANCHES_COUNT = 3;

		
	};

	
}
#endif

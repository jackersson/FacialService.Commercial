#ifndef FacialVerification_INCLUDED
#define FacialVerification_INCLUDED

#include "biotasks\ifir_builder.hpp"
#include "feedback\facial_verification_feedback.hpp"
#include "common\matches.hpp"
#include <wrappers/face_info.hpp>
#include <common/verification_result.hpp>
#include <wrappers/image_info.hpp>
#include <utils/pipeline_governor.hpp>
#include <mutex>


namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Verification::Processor> FRsdkVerificationProcessor;
	typedef FRsdk::CountedPtr<FRsdk::ScoreMappings>           FRsdkScoreMappings        ;
	
	//typedef std::pair<FRsdkTypes::FaceVacsImage, FRsdkFir>  VerificationWorkUnit;
	//typedef std::shared_ptr<VerificationWorkUnit>           VerificationWorkUnitPtr;

	class VerificationWorkUnit
	{
	public:
		VerificationWorkUnit(FRsdkTypes::FaceVacsImage image, FRsdkFir fir) : image_(image), fir_(fir)
		{}

		FRsdkTypes::FaceVacsImage image()	const {
			return image_;
		}

		FRsdkFir fir()	const {
			return fir_;
		}

		void done( float score ){
			asend(score_, score);
		}

		void wait( )	{
			receive(&score_);
		}

		float score()	{
			return score_.has_value() ? score_.value() : 0.0f;
		}
	private:
		FRsdkTypes::FaceVacsImage image_;
		FRsdkFir                  fir_  ;

		VerificationWorkUnit(const VerificationWorkUnit&);
		VerificationWorkUnit const & operator=(VerificationWorkUnit const&);

		Concurrency::single_assignment<float> score_;
	};


	typedef std::shared_ptr<VerificationWorkUnit> VerificationWorkUnitPtr;

	//TODO make a part of verifyer
	class FacialVerificationProcessor //: public IVerificationProcessor
	{
		typedef Concurrency::transformer<VerificationWorkUnitPtr, float> VerificationProcessorTransformer;

	public:		
		explicit FacialVerificationProcessor(FRsdkTypes::FaceVacsConfiguration configuration) : locked_(false)
		{
			concurrency::parallel_invoke
			([&]()
			{
				try  {
					processor_ = new FRsdk::Verification::Processor(*configuration);
				}
				catch (std::exception& e) {
					std::cout << e.what();
				}
			},
				[this](){	init();	}
			);
		}

		~FacialVerificationProcessor(){}	

		bool locked() const {
			return locked_;
		}

		void lock()	{
			locked_ = true;
		}

		void unlock(){
			locked_ = false;
		}

		template<typename T>
		void link_to(Concurrency::unbounded_buffer<T>& buffer) const	{
			buffer.link_target(transformer_.get());
		}

	private:
		void init()
		{			
			transformer_ = std::make_unique<VerificationProcessorTransformer>(
				[this](VerificationWorkUnitPtr pInfo) -> float
			  {
			  	lock();
			  	auto score = verify(pInfo->image(), pInfo->fir());	
					pInfo->done(score);
				  unlock();
					return score;
		  	},
				nullptr,
				[this](VerificationWorkUnitPtr pInfo) -> bool  {
				  return !locked();
			  }
			);
			
		}
		
		float verify(FRsdkTypes::FaceVacsImage image, FacialFeedback::FirPtr fir) const
		{
			FRsdk::SampleSet images;
			images.push_back(*image);

			FRsdk::CountedPtr<FRsdk::Score> result(new FRsdk::Score(.0f));

			FRsdk::CountedPtr<FacialVerificationFeedback> verification_feedback(new FacialVerificationFeedback());

			FRsdk::Verification::Feedback
				feedback(verification_feedback);

			processor_->process(images.begin(), images.end(), *fir, 0.01f, feedback);
			
			return verification_feedback->result();
		}

		FacialVerificationProcessor(const FacialVerificationProcessor&);
		FacialVerificationProcessor const & operator=(FacialVerificationProcessor const&);
			
		FRsdkVerificationProcessor processor_;

		bool locked_;
		std::mutex mutex_;

		std::unique_ptr<VerificationProcessorTransformer> transformer_;

		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
		const float FAR_THRESHOLD    = 0.001f;
	};

	typedef std::shared_ptr<FacialVerificationProcessor> FacialVerificationProcessorPtr;

	class VerificationLoadBalancer : public Pipeline::PipelineGovernor
	{
	public:
		explicit VerificationLoadBalancer( FRsdkTypes::FaceVacsConfiguration configuration
			                               , int count = MIN_PROCESSORS_COUNT)
			                               : Pipeline::PipelineGovernor(0)
			                               , configuration_(configuration)
		{
			init(count);
		}

		void resize(int count) override {
			init(count);
		}

		float verify(FRsdkTypes::FaceVacsImage image, FRsdkFir fir)
		{
			VerificationWorkUnitPtr unit(new VerificationWorkUnit(image, fir));
			wait_available_working_unit();
			concurrency::asend(inputs_, unit);
			unit->wait();
			free_unit();
			return unit->score();
		}

		void stop()	{
			wait_until_empty();
		}

	private:
		VerificationLoadBalancer(const VerificationLoadBalancer&);
		VerificationLoadBalancer const & operator=(VerificationLoadBalancer const&);

		void init(int count)
		{
			auto available_count = MAX_PROCESSORS_COUNT - capacity_;
			if (available_count <= 0 || available_count <= count)
			{
				std::cout << "max count of verification load balancer reached" << std::endl;
				return;
			}

			concurrency::parallel_for(0, MAX_PROCESSORS_COUNT, count,
				[&](int i)
			{
				FacialVerificationProcessorPtr ptr(new FacialVerificationProcessor(configuration_));
				if (ptr != nullptr)
				{
					ptr->link_to<VerificationWorkUnitPtr>(inputs_);
					processors_.push_back(ptr);
				}
			}
			);
			Pipeline::PipelineGovernor::resize(processors_.size());
		}
		FRsdkTypes::FaceVacsConfiguration configuration_;

		concurrency::unbounded_buffer<VerificationWorkUnitPtr>  inputs_;
		std::list<FacialVerificationProcessorPtr>                processors_;

		static const int MIN_PROCESSORS_COUNT = 1;
		static const int MAX_PROCESSORS_COUNT = 3;
	};

	typedef std::shared_ptr<VerificationLoadBalancer> VerificationLoadBalancerPtr;

	class VerificationItem
	{
		typedef std::pair<FRsdkTypes::FaceVacsImage, long>  VerificationObject;
		typedef std::pair<FRsdk::FIR, long>  VerificationSubject;
		
	public:
		explicit VerificationItem( FRsdkEntities::ImageInfoPtr object
			                       , VerificationLoadBalancerPtr load_balancer) : load_balancer_(load_balancer)
		                                                                      , counter_(0)			    
		{
			Concurrency::parallel_for_each(object->cbegin(), object->cend(),
				[&](FRsdkEntities::FaceInfoPtr face)
			  {
			  	container_.push_back(face);
			  }
			);
		}

		const BioContracts::Matches& verify( FRsdkEntities::ImageInfoPtr object )			                                 
		{			
			Concurrency::parallel_for_each(object->cbegin(), object->cend(),
				[&](FRsdkEntities::FaceInfoPtr face)
			  {
			  	process(face);
			  }
			);

			return matches_;
		}

		void process(FRsdkEntities::FaceInfoPtr item)
		{
			if (!item->has_fir())
				return;

			VerificationObject  verification_object(item->extracted_image(), item->id());

			concurrency::parallel_for_each(container_.cbegin(), container_.cend(),
				[&](FRsdkEntities::FaceInfoPtr iter_item)
			{
				//if (item->image_id() == iter_item->image_id())
					//return;

				++counter_;

				auto match_score = load_balancer_->verify(iter_item->extracted_image(), item->enrollment_data()->fir());
				
				if (match_score  > 0.5f )
					matches_.add(item->id(), BioContracts::Match(iter_item->id(), match_score));
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

		//TODO remove when not need
		int counter() const	{
			return counter_;
		}

		size_t size() const	{
			return matches_.size();
		}

	private:
		VerificationItem(const VerificationItem&);
		VerificationItem const & operator=(VerificationItem const&);

		std::list<FRsdkEntities::FaceInfoPtr> container_;
		BioContracts::Matches matches_;

		VerificationLoadBalancerPtr    load_balancer_;
		FRsdkScoreMappings             score_mappings_;
		FirBuilderRef                  fir_builder_;


		int counter_;
	};
	typedef std::shared_ptr<VerificationItem> VerificationItemPtr;


	typedef std::pair<FRsdkEntities::ImageInfoPtr, FRsdkEntities::ImageInfoPtr> VerificationPair;
	class FacialVerification
	{
	public:
		explicit FacialVerification(const std::string& configuration_filename);
		

		explicit FacialVerification(FRsdkTypes::FaceVacsConfiguration configuration);

		BioContracts::Matches verify(VerificationPair pair)
		{
			auto ptr = create_verification_item(pair.first);
			ptr->verify(pair.second);
			return ptr->matches();
		}

		VerificationItemPtr create_verification_item(FRsdkEntities::ImageInfoPtr image)
		{
			return VerificationItemPtr(std::make_shared<VerificationItem>(image, load_balancer_));
		}

	private:
		FacialVerification(const FacialVerification&);
		FacialVerification const & operator=(FacialVerification const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);


		VerificationLoadBalancerPtr load_balancer_;
	};

	typedef std::shared_ptr<FacialVerification> FacialVerificationPtr;

}

#endif
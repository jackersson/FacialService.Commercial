#ifndef FacialIdentification_INCLUDED
#define FacialIdentification_INCLUDED

#include "biotasks\ifir_builder.hpp"
//#include "feedback\facial_verification_feedback.hpp"
#include "common\matches.hpp"
#include <wrappers/face_info.hpp>
#include <wrappers/image_info.hpp>
#include <utils/pipeline_governor.hpp>
#include <mutex>
#include <frsdk/ident.h>
#include <feedback/facial_identification_feedback.hpp>


namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Identification::Processor> FRsdkIdentificationProcessor;
  typedef std::shared_ptr<FRsdk::Population>                  FRsdkPopulationPtr          ;
	
	class IdentificationWorkUnit
	{
	public:
		explicit IdentificationWorkUnit(FRsdkTypes::FaceVacsImage image) : image_(image)
		{}

		FRsdkTypes::FaceVacsImage image()	const {
			return image_;
		}
		
		void done(BioContracts::MatchSetPtr score){
			asend(score_, score);
		}

		void wait()	{
			receive(&score_);
		}

		BioContracts::MatchSetPtr score()	{
			return score_.has_value() ? score_.value() : nullptr;
		}
	private:
		FRsdkTypes::FaceVacsImage image_;
	
		IdentificationWorkUnit(const IdentificationWorkUnit&);
		IdentificationWorkUnit const & operator=(IdentificationWorkUnit const&);

		Concurrency::single_assignment<BioContracts::MatchSetPtr> score_;
	};


	typedef std::shared_ptr<IdentificationWorkUnit> IdentificationWorkUnitPtr  ;
	typedef std::pair<long, float>                  IdentificationProbability  ;
	typedef std::list<IdentificationProbability>    IdentificationProbabilities;
	//TODO make a part of verifyer
	class FacialIdentificationProcessor //: public IVerificationProcessor
	{
		typedef Concurrency::transformer< IdentificationWorkUnitPtr
	                               		, BioContracts::MatchSetPtr> IdentificationProcessorTransformer;

	public:
		explicit FacialIdentificationProcessor( FRsdkTypes::FaceVacsConfiguration configuration
			                                    , FRsdkPopulationPtr population ) 
																					: locked_(false)			                                   
			                                    , processor_(nullptr)
		{			
			concurrency::parallel_invoke(
				[&]()
			  {
			  	try  {
			  		processor_ = new FRsdk::Identification::Processor(*configuration, *population);
			  	}
			  	catch (std::exception& e) {
			  		std::cout << e.what();
			  	}
			  },
				[&]()	{	init();	}
			);				
		}

		~FacialIdentificationProcessor(){}
		
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
			  transformer_ = std::make_unique<IdentificationProcessorTransformer>(
					[this](IdentificationWorkUnitPtr pInfo) -> BioContracts::MatchSetPtr
			    {
			    	lock();
			    	auto score = identify(pInfo->image());
			    	pInfo->done(score);
			    	unlock();
			    	return score;
			    },
			  	nullptr,
			  	[this](IdentificationWorkUnitPtr pInfo) -> bool  {
			  	return !locked();
			  }
			);
		}

		BioContracts::MatchSetPtr identify(FRsdkTypes::FaceVacsImage image) const
		{
			FRsdk::SampleSet images;
			images.push_back(*image);
						
			FRsdk::CountedPtr<FacialIdentificationFeedback> identification_feedback(new FacialIdentificationFeedback());

			FRsdk::Identification::Feedback
				feedback(identification_feedback);

			processor_->process(images.begin(), images.end(), 0.01f, feedback, MAX_MATCHES_COUNT);
			std::cout << "here 1" << std::endl;
			return std::make_shared<BioContracts::MatchSet>(identification_feedback->result());
		}

		FacialIdentificationProcessor(const FacialIdentificationProcessor&);
		FacialIdentificationProcessor const & operator=(FacialIdentificationProcessor const&);

		FRsdkIdentificationProcessor processor_;

		bool locked_;
		std::mutex mutex_;
	
		std::unique_ptr<IdentificationProcessorTransformer> transformer_;

		const float MIN_EYE_DISTANCE         = 0.1f;
		const float MAX_EYE_DISTANCE         = 0.4f;
		const float FAR_THRESHOLD            = 0.001f;
		const unsigned int MAX_MATCHES_COUNT = 3;
	};

	typedef std::shared_ptr<FacialIdentificationProcessor> FacialIdentificationProcessorPtr;

	class IdentificationLoadBalancer : public Pipeline::PipelineGovernor
	{
	public:
		explicit IdentificationLoadBalancer( FRsdkTypes::FaceVacsConfiguration configuration
			                                 , FRsdkPopulationPtr population
			                                 , int count = MIN_PROCESSORS_COUNT)
			                                 : Pipeline::PipelineGovernor(0)
			                                 , configuration_(configuration)
																			 , population_(population)
		{
			count = 1;
			init(count);
		}

		void resize(int count) override {
			init(count);
		}


		BioContracts::MatchSetPtr identify(FRsdkTypes::FaceVacsImage image)
		{
			auto unit(std::make_shared<IdentificationWorkUnit>(image));
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
		IdentificationLoadBalancer(const IdentificationLoadBalancer&);
		IdentificationLoadBalancer const & operator=(IdentificationLoadBalancer const&);

		void init(int count)
		{
			auto available_count = MAX_PROCESSORS_COUNT - capacity_;
			if (available_count <= 0 || available_count <= count)
			{
				std::cout << "max count of verification load balancer reached" << std::endl;
				return;
			}

			concurrency::parallel_for(0, count, 1,
				[&](int i)
			{
				auto ptr(std::make_shared<FacialIdentificationProcessor>(configuration_, population_));
				if (ptr != nullptr)
				{
					ptr->link_to<IdentificationWorkUnitPtr>(inputs_);
					processors_.push_back(ptr);
				}
			}
			);
			Pipeline::PipelineGovernor::resize(processors_.size());
		}
		FRsdkTypes::FaceVacsConfiguration configuration_;
		FRsdkPopulationPtr                population_;

		concurrency::unbounded_buffer<IdentificationWorkUnitPtr>  inputs_;
		std::list<FacialIdentificationProcessorPtr>               processors_;

		static const int MIN_PROCESSORS_COUNT = 1;
		static const int MAX_PROCESSORS_COUNT = 3;
	};

	typedef std::shared_ptr<IdentificationLoadBalancer> IdentificationLoadBalancerPtr;

	class IdentificationItem
	{
		typedef std::pair<FRsdkTypes::FaceVacsImage, long>  VerificationObject;
		typedef std::pair<FRsdk::FIR, long>  VerificationSubject;
	public:
		explicit IdentificationItem( const std::list<FRsdkEntities::ImageInfoPtr>& images			
			                         , FRsdkTypes::FaceVacsConfiguration configuration) 
															 : load_balancer_(nullptr)
															 , matches_(std::make_shared<BioContracts::Matches>())
															 , configuration_(configuration)
			                         , counter_(0)
															 
		{
			init(images);			
		}

		BioContracts::MatchesPtr identify(FRsdkEntities::ImageInfoPtr image)
		{			
			
			Concurrency::parallel_for_each(image->cbegin(), image->cend(),
				[&](FRsdkEntities::FaceInfoPtr face)
			  {					
					auto matches = load_balancer_->identify(face->extracted_image());				
					matches_->add(face->id(), *matches);
			  	container_.push_back(face);
			  }
			);

			return matches_;
		}
		
		void clear()	{
			container_.clear();
			matches_->clear();
		}

		BioContracts::MatchesPtr matches() const	{
			return matches_;
		}

		//TODO remove when not need
		int counter() const	{
			return counter_;
		}

		size_t size() const	{
			return matches_->size();
		}

	private:

		void init(const std::list<FRsdkEntities::ImageInfoPtr>& images)
		{			
			auto population(std::make_shared<FRsdk::Population>(*configuration_));

			Concurrency::parallel_for_each(images.cbegin(), images.cend(),
				[&](FRsdkEntities::ImageInfoPtr image)
			  {
			  	Concurrency::parallel_for_each(image->cbegin(), image->cend(),
			  		[&](FRsdkEntities::FaceInfoPtr face)
			  	  {
			  	  	population->append(*(face->enrollment_data()->fir()), std::to_string((face->id())));
			  	  }
			  	);
			  }
			);

			load_balancer_ = std::make_shared<IdentificationLoadBalancer>(configuration_, population);
		}


		IdentificationItem(const IdentificationItem&);
		IdentificationItem const & operator=(IdentificationItem const&);

		std::list<FRsdkEntities::FaceInfoPtr> container_;
		BioContracts::MatchesPtr  matches_;

		IdentificationLoadBalancerPtr   load_balancer_;
//		FRsdkScoreMappings             score_mappings_;
		FirBuilderRef                  fir_builder_;
		FRsdkTypes::FaceVacsConfiguration configuration_;

		int counter_;
	};
	typedef std::shared_ptr<IdentificationItem> IdentificationItemPtr;


	typedef std::pair<FRsdkEntities::ImageInfoPtr, std::list<FRsdkEntities::ImageInfoPtr>> IdentificationPair;
	class FacialIdentification
	{
	public:
		explicit FacialIdentification(const std::string& configuration_filename);


		explicit FacialIdentification(FRsdkTypes::FaceVacsConfiguration configuration);

		BioContracts::MatchesPtr identify(IdentificationPair pair)
		{
			auto ident_item(std::make_shared<IdentificationItem>(pair.second, configuration_));
			
			auto matches = ident_item->identify(pair.first);
		
			return matches;
		}		

	private:
		FacialIdentification(const FacialIdentification&);
		FacialIdentification const & operator=(FacialIdentification const&);

		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

		FRsdkTypes::FaceVacsConfiguration configuration_;
		
	};

	typedef std::shared_ptr<FacialIdentification> FacialIdentificationPtr;

}

#endif
#ifndef FacialVerification_INCLUDED
#define FacialVerification_INCLUDED

#include "biotasks\ifir_builder.hpp"
#include "feedback\facial_verification_feedback.hpp"
#include "common\matches.hpp"
#include <wrappers/face_info.hpp>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Verification::Processor> FRsdkVerificationProcessor;
	typedef FRsdk::CountedPtr<FRsdk::ScoreMappings>           FRsdkScoreMappings        ;


	class Veryfier
	{
		typedef std::pair<FRsdkTypes::FaceVacsImage, long>  VerificationObject;
		typedef std::pair<FRsdk::FIR, long>  VerificationSubject;
	public:
		explicit Veryfier(const std::string& configuration_filename)
		{
			try
			{
				FRsdkTypes::FaceVacsConfiguration configuration(new FRsdk::Configuration(configuration_filename));
				init(configuration);
			}
			catch (std::exception& e) {
				std::cout << e.what();
			}
		}

		explicit Veryfier(FRsdkTypes::FaceVacsConfiguration configuration)
		{
			init(configuration);
		}

		bool init(FRsdkTypes::FaceVacsConfiguration configuration)
		{
			try
			{
				processor_      = new FRsdk::Verification::Processor(*configuration);
				score_mappings_ = new FRsdk::ScoreMappings(*configuration);
				fir_builder_    = std::make_shared<FirBilder>(configuration);
				return true;
			}
			catch (std::exception& e) { std::cout << e.what(); return false; }
		}

		void Add(FRsdkEntities::FaceInfoPtr item)
		{
			if (!item->has_fir() && item->has_extracted_image())
			{
				container_.push_back(item);
				return;
			}

			VerificationObject  verification_object(item->extracted_image(), item->id());

			concurrency::parallel_for_each(container_.cbegin(), container_.cend(),
				[&](FRsdkEntities::FaceInfoPtr iter_item)
			  {
			  	if (item->image_id() == iter_item->image_id())
			  		return;
			  
			  	++counter_;
			  
			  	VerificationSubject verification_subject(iter_item->enrollment_data()->fir(), item->id());
			  
			  	auto match(verify(verification_object, verification_subject));
					auto match_score = match.match() > 0.5f;
			  
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

		//TODO remove when not need
		int counter() const	{
			return counter_;
		}

		size_t size() const	{
			return matches_.size();
		}

	private:
		BioContracts::Match verify( const VerificationObject & image
			                        , const VerificationSubject& fir)	const
		{
			FRsdk::SampleSet images;
			images.push_back(*(image.first));

			FRsdk::CountedPtr<FacialVerificationFeedback> verification_feedback
				= new FacialVerificationFeedback(BioContracts::Match(image.second, fir.second, 0.0f));

			FRsdk::Verification::Feedback
				feedback(verification_feedback);

			//std::cout << fir.size() << std::endl;

			processor_->process(images.begin(), images.end()
				                  , fir.first
				                  , 0.01f
				                  , feedback);

			return verification_feedback->result();
		}

	private:
		std::list<FRsdkEntities::FaceInfoPtr> container_;
		BioContracts::Matches matches_;

		FRsdkVerificationProcessor processor_;
		FRsdkScoreMappings         score_mappings_;
		FirBuilderRef              fir_builder_;


		int counter_;
	};

	typedef std::shared_ptr<Veryfier> VeryfierPtr;


	//TODO make a part of verifyer

	class FacialVerification //: public IVerificationProcessor
	{
	public:
		explicit FacialVerification(const std::string& configuration_filename);
		explicit FacialVerification(FRsdkTypes::FaceVacsConfiguration configuration);
		~FacialVerification(){}
	
		float verify(const FRsdk::Image& image, const FRsdk::FIR& fir);

		FRsdkFir build(const std::string& bytes) const;

	private:
		bool init(FRsdkTypes::FaceVacsConfiguration configuration);

	
		FRsdkVerificationProcessor processor_     ;
		FRsdkScoreMappings         score_mappings_;
		FirBuilderRef              fir_builder_   ;

		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;

		const float FAR_THRESHOLD    = 0.001f;
	};

	typedef std::shared_ptr<FacialVerification> FacialVerificationPtr;
}

#endif
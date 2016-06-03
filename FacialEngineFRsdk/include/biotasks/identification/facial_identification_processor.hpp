#ifndef FacialIdentificationProcessor_INCLUDED
#define FacialIdentificationProcessor_INCLUDED

//#include "biotasks\ifir_builder.hpp"

//#include "common\matches.hpp"
//#include <wrappers/face_info.hpp>

#include <mutex>
#include <frsdk/ident.h>

#include "identification_work_unit.hpp"
#include <agents.h>


namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Identification::Processor> FRsdkIdentificationProcessor;
	typedef std::shared_ptr<FRsdk::Population>                  FRsdkPopulationPtr;
	
	typedef std::pair<long, float>                  IdentificationProbability;
	typedef std::list<IdentificationProbability>    IdentificationProbabilities;
	
	class FacialIdentificationProcessor 
	{
		typedef Concurrency::transformer< IdentificationWorkUnitPtr
		                                  , BioContracts::MatchSet> IdentificationProcessorTransformer;

	public:
		explicit FacialIdentificationProcessor( FRsdkTypes::FaceVacsConfiguration configuration
			                                    , FRsdkPopulationPtr population);
		~FacialIdentificationProcessor(){}

		bool locked() const;
		void lock();
		void unlock();

		template<typename T>
		void link_to(Concurrency::unbounded_buffer<T>& buffer) const	{
			buffer.link_target(transformer_.get());
		}

	private:
		void init();
	

		BioContracts::MatchSet identify(FRsdkTypes::FaceVacsImage image) const;
		

		FacialIdentificationProcessor(const FacialIdentificationProcessor&);
		FacialIdentificationProcessor const & operator=(FacialIdentificationProcessor const&);

		FRsdkIdentificationProcessor processor_;

		bool locked_;
		std::mutex mutex_;

		std::unique_ptr<IdentificationProcessorTransformer> transformer_;

		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
		const float FAR_THRESHOLD = 0.001f;
		const unsigned int MAX_MATCHES_COUNT = 3;
	};

	typedef std::shared_ptr<FacialIdentificationProcessor> FacialIdentificationProcessorPtr;
}
#endif
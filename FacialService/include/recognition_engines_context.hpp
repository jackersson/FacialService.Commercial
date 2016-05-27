#ifndef RecognitionEnginesContext_INCLUDED
#define RecognitionEnginesContext_INCLUDED

#include "engine/irecognition_engines_context.hpp"
#include "facial_engine.hpp"

namespace BioFacialService
{
	class RecognitionEnginesContext : public BioContracts::IRecognitionEnginesContext
	{
		
	public:

		RecognitionEnginesContext(){}
		virtual ~RecognitionEnginesContext(){}

		void createFacialEngine(const std::string& configuration_filename)
		{		
			facial_engine_.reset();			
			facial_engine_ = std::make_shared<BioFacialEngine::FacialEngine>(configuration_filename);
		}

		BioContracts::IFacialEnginePtr facial_engine() override
		{
			return facial_engine_;
		}

	private:
		RecognitionEnginesContext(const RecognitionEnginesContext&);
		RecognitionEnginesContext& operator=(const RecognitionEnginesContext&);

		BioFacialEngine::FacialEnginePtr facial_engine_;
	};
}

#endif
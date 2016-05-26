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
		~RecognitionEnginesContext(){}

		void createFacialEngine(const std::string& configuration_filename)
		{
			if (hasFacialEngine())
				facial_engine_.reset();

			//std::shared_ptr<BioFacialEngine::FacialEngine> facial_engine(
			//	new BioFacialEngine::FacialEngine(configuration_filename));
			//std::string test = "";
			//BioFacialEngine::FacialEngine* fc = new BioFacialEngine::FacialEngine(configuration_filename);
			facial_engine_ = std::make_shared<BioFacialEngine::FacialEngine>(configuration_filename);
		}

		bool hasFacialEngine() const
		{
			return facial_engine_.get() != NULL;
		}

		std::shared_ptr<BioContracts::IFacialEngine> facialEngine() const
		{
			return nullptr;
		}

	private:
		RecognitionEnginesContext(const RecognitionEnginesContext&);
		RecognitionEnginesContext& operator=(const RecognitionEnginesContext&);

	private:
		std::shared_ptr<BioFacialEngine::FacialEngine> facial_engine_;

	};
}

#endif
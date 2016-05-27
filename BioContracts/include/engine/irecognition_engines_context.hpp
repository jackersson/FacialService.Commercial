#ifndef IRecognitionEnginesContext_INCLUDED
#define IRecognitionEnginesContext_INCLUDED

#include "ifacial_engine.hpp"
namespace BioContracts
{
	typedef std::shared_ptr<IFacialEngine> IFacialEnginePtr;
	class IRecognitionEnginesContext
	{
	public:
		virtual ~IRecognitionEnginesContext() {}

		virtual IFacialEnginePtr facial_engine()  = 0;		
	};


}

#endif
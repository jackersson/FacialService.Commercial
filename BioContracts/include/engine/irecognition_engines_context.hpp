#ifndef IRecognitionEnginesContext_INCLUDED
#define IRecognitionEnginesContext_INCLUDED

#include "ifacial_engine.hpp"
namespace BioContracts
{
	class IRecognitionEnginesContext
	{
	public:
		virtual std::shared_ptr<IFacialEngine> facialEngine   () const = 0;
		virtual bool                           hasFacialEngine() const = 0;
	};


}

#endif
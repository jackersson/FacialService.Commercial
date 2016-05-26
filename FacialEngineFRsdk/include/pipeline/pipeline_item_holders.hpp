#ifndef PipelineItemHolders_INCLUDED
#define PipelineItemHolders_INCLUDED

#include "utils/pipeline_governor.hpp"

namespace Pipeline
{
	enum BiometricTask : long
  {
  	  FaceFind             = 1 << 0
  	, PortraitAnalysis     = 1 << 1
  	, FaceImageExtraction  = 1 << 2
  	, IsoComplianceTest    = 1 << 4
  	, Enrollment           = 1 << 8
  	, Verification         = 1 << 16
  };

	template <typename T>
	class PipelineAwaitable : public PipelineGovernor
	{
	public:
		explicit PipelineAwaitable( T ptr )
			                         : PipelineGovernor(ptr->size())
			                         , item_(ptr)
		{}

		virtual ~PipelineAwaitable() {}

	protected:
		T item_;

	private:
		PipelineAwaitable(const PipelineAwaitable&);
		PipelineAwaitable const & operator=(PipelineAwaitable const&);		
	};
}
#endif

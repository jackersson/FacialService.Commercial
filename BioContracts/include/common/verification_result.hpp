#ifndef VerificationResult_INCLUDED
#define VerificationResult_INCLUDED

#include "common\matches.hpp"
#include "common\iimage_characteristic.hpp"

namespace BioContracts
{	
	class VerificationResult
	{
	public:
		VerificationResult( const Matches& matches
			                ,  IImageInfoPtr first
											,  IImageInfoPtr  second)
											: matches_(matches)
											, p(first, second)										
		{}

		const Matches& matches() const { return matches_; }
		IImageInfoPtr  first() const { return p.first; }
		IImageInfoPtr  second() const { return p.second; }
	private:
		Matches matches_;
		std::pair<IImageInfoPtr, IImageInfoPtr> p;
	};

	typedef std::shared_ptr<VerificationResult> VerificationResultPtr;
}

#endif
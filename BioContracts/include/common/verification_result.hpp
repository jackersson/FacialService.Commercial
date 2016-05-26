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
			                , ImageCharacteristicsConstRef first
											, ImageCharacteristicsConstRef second) 
											: matches_(matches)
											, p(first, second)										
		{}

		const Matches& matches() const { return matches_; }
		ImageCharacteristicsConstRef first() const { return p.first; }
		ImageCharacteristicsConstRef second() const { return p.second; }
	private:
		Matches matches_;
		std::pair<ImageCharacteristicsConstRef, ImageCharacteristicsConstRef> p;
	};
}

#endif
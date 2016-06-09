#ifndef VerificationWorkUnit_INCLUDED
#define VerificationWorkUnit_INCLUDED

#include "biotasks\ifir_builder.hpp"
#include "common\matches.hpp"
#include <agents.h>

namespace BioFacialEngine
{	
	class VerificationWorkUnit
	{
	public:
		VerificationWorkUnit(FRsdkTypes::FaceVacsImage image, FRsdkFir fir) : image_(image), fir_(fir)
		{}

		FRsdkTypes::FaceVacsImage image()	const {
			return image_;
		}

		FRsdkFir fir()	const {
			return fir_;
		}

		void done(float score){
			asend(score_, score);
		}

		void wait()	{
			receive(&score_);
		}

		float score()	{
			return score_.has_value() ? score_.value() : 0.0f;
		}
	private:
		FRsdkTypes::FaceVacsImage image_;
		FRsdkFir                  fir_;

		VerificationWorkUnit(const VerificationWorkUnit&);
		VerificationWorkUnit const & operator=(VerificationWorkUnit const&);

		Concurrency::single_assignment<float> score_;
	};

	typedef std::shared_ptr<VerificationWorkUnit> VerificationWorkUnitPtr;
}
#endif
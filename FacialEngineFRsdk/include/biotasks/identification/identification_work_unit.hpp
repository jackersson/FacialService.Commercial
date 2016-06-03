#ifndef IdentificationWorkUnit_INCLUDED
#define IdentificationWorkUnit_INCLUDED

#include <wrappers/face_info.hpp>
#include <common/matches.hpp>
#include <agents.h>

namespace BioFacialEngine
{	
	class IdentificationWorkUnit
	{
	public:
		explicit IdentificationWorkUnit(FRsdkTypes::FaceVacsImage image) : image_(image)
		{}

		FRsdkTypes::FaceVacsImage image()	const {
			return image_;
		}

		void done(BioContracts::MatchSet score){
			asend(score_, score);
		}

		void wait()	{
			receive(&score_);
		}

		BioContracts::MatchSet score()	{
			return score_.value();
		}
	private:
		FRsdkTypes::FaceVacsImage image_;

		IdentificationWorkUnit(const IdentificationWorkUnit&);
		IdentificationWorkUnit const & operator=(IdentificationWorkUnit const&);

		Concurrency::single_assignment<BioContracts::MatchSet> score_;
	};


	typedef std::shared_ptr<IdentificationWorkUnit> IdentificationWorkUnitPtr;
}
#endif
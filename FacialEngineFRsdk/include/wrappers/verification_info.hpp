#ifndef VerificationInfo_INCLUDED
#define VerificationInfo_INCLUDED

#include "wrappers/face_info.hpp"
#include "wrappers/frsdk_face_characteristics.hpp"
#include <common/iimage_characteristic.hpp>

namespace FRsdkEntities
{
	class VerificationInfo : public BioContracts::IImageInfo
	{
	public:
		explicit VerificationInfo(){}



	private:
		VerificationInfo(const VerificationInfo& rhs);
		VerificationInfo const & operator=(VerificationInfo const&);

	};

	typedef std::shared_ptr<VerificationInfo>    VerificationInfoPtr;
}
#endif
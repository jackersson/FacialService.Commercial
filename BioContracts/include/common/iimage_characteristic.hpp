#ifndef IImageInfo_INCLUDED
#define IImageInfo_INCLUDED

#include "common/iface_characteristics.hpp"
#include <string>

namespace BioContracts
{

	class IFaceInfo
	{
	public:
		virtual ~IFaceInfo() {}

		virtual const IFaceCharacteristics& characteristics() const = 0;
		virtual const IComlianceIsoTemplate& iso_compliance() const = 0;

		virtual const IFace& face() const = 0;
		virtual const IEyes& eyes()	const = 0;

		virtual std::string identification_record() const = 0;
	};

	class IImageInfo
	{
	public:
		virtual ~IImageInfo() {}

		virtual size_t size() const = 0;
		virtual const IFaceInfo& operator[](size_t index) const = 0;	
	};

	typedef std::shared_ptr<IFaceInfo>  IFaceInfoPtr ;
	typedef std::shared_ptr<IImageInfo> IImageInfoPtr;
}

#endif
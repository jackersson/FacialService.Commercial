#ifndef IImageCharacteristics_INCLUDED
#define IImageCharacteristics_INCLUDED

#include "service_context.hpp"
#include "iface_characteristics.hpp"

namespace BioContracts
{

	class IImageCharacteristics
	{
	public:
		//typedef const BioContracts::IImageCharacteristics& ImageCharacteristicsConstRef;

		virtual size_t size() const = 0;
		virtual const IFaceCharacteristics& operator[](size_t index) const = 0;
	
	};

	typedef std::shared_ptr<BioContracts::IImageCharacteristics> ImageCharacteristicsConstRef;
}

#endif
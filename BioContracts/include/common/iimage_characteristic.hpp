#ifndef IImageInfo_INCLUDED
#define IImageInfo_INCLUDED

#include "common/iface_characteristics.hpp"
#include <string>
#include <list>
#include <ppl.h>

namespace BioContracts
{

	class IFaceInfo
	{
	public:
		virtual ~IFaceInfo() {}

		virtual bool has_face_characteristics() const = 0;
		virtual bool has_iso_compliance()       const = 0;

		virtual long id() const = 0;

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
		virtual long id() const = 0;
		virtual size_t size() const = 0;
		virtual const IFaceInfo& operator[](size_t index) const = 0;	
	};

	typedef std::shared_ptr<IFaceInfo>  IFaceInfoPtr ;
	typedef std::shared_ptr<IImageInfo> IImageInfoPtr;

	class IImageInfoSet
	{
	public:
	  IImageInfoSet(){}		
		template <typename Iter>
		void set_identification_images(Iter begin, Iter end)
		{
			Concurrency::parallel_for_each(begin, end,
				[&](IImageInfoPtr image){
				images_.push_back(image);
			}
			);
		}

		std::list<IImageInfoPtr>::const_iterator cbegin() const {
			return images_.cbegin();
		}

		std::list<IImageInfoPtr>::const_iterator cend() const {
			return images_.cend();
		}

		size_t size() const
	  {
			return images_.size();
	  }

	private:	
		std::list<IImageInfoPtr> images_;
	};	

	//typedef std::list<IImageInfoPtr> IImageInfoSet;
}

#endif
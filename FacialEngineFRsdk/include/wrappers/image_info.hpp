#ifndef ImageInfo_INCLUDED
#define ImageInfo_INCLUDED

#include "wrappers/face_info.hpp"
#include "wrappers/frsdk_face_characteristics.hpp"
#include <utils/face_vacs_io_utils.hpp>
#include <common/iimage_characteristic.hpp>

namespace FRsdkEntities
{
	class ImageInfo : public BioContracts::IImageInfo
	{
	public:
		explicit ImageInfo(FRsdkTypes::FaceVacsImage image, long id) : image_(image), id_(id){}

		long id() const override {
			return id_;
		}

		FRsdkTypes::FaceVacsImage image() const {
			return image_;
		}

		void addRange(const std::vector<FaceVacsFullFace>& faces)
		{
			Concurrency::parallel_for_each(faces.begin(), faces.end(),
				[&](const FaceVacsFullFace& face) 
			  {
			  	addFace(face.first, face.second);
			  }
			);
		}

		void addFace( const FRsdk::Face::Location& faceLocation
			          , const FRsdk::Eyes::Location& eyes)
		{
			FaceInfoPtr face(new FaceInfo(faceLocation, eyes, image_));
			faces_.push_back(face);
		}

		size_t size() const override	{
			return faces_.size();
		}
	
		std::vector<FaceInfoPtr>::const_iterator cbegin() const{
			return faces_.cbegin();
		}

		std::vector<FaceInfoPtr>::const_iterator cend() const{
			return faces_.cend();
		}
		
		const BioContracts::IFaceInfo& operator[](size_t index) const override
		{
			if (index >= size())
				throw std::exception("Index out of range exception");

			auto ptr = faces_[index];

			return *ptr;
		}
		

	private:
		ImageInfo(const ImageInfo& rhs);
		ImageInfo const & operator=(ImageInfo const&);
	
		std::vector<FaceInfoPtr> faces_;
		FRsdkTypes::FaceVacsImage image_;

		long id_;
	};

	typedef std::shared_ptr<ImageInfo>    ImageInfoPtr;
}
#endif
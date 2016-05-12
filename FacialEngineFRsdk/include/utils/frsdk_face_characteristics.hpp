#ifndef FRsdkFaceCharacteristics_INCLUDED
#define FRsdkFaceCharacteristics_INCLUDED


#include "engine/ifacial_engine.hpp"
#include "common/iface_characteristics.hpp"
#include "common/iimage_characteristic.hpp"

#include "feedback/facial_acquisiton_feedback.hpp"
#include "feedback/facial_enrollment_feedback.hpp"
#include "feedback/facial_verification_feedback.hpp"

#include <fstream>

#include <frsdk/config.h>
#include <frsdk/image.h>
#include <frsdk/tokenface.h>
#include <frsdk/portrait.h>
#include <frsdk/eyes.h>
#include <frsdk/cbeff.h>
#include <frsdk/enroll.h>
#include <frsdk/verify.h>

#include <frsdk/jpeg.h>
#include <frsdk/j2k.h>
#include <frsdk/bmp.h>
#include <frsdk/png.h>
#include <frsdk/pgm.h>

#include <frsdk/portraittests.h>

#include "biotasks/ienrollment_processor.hpp"
#include "biotasks/ienrollment_able.hpp"

namespace BioFacialEngine
{
	class ImageCharacteristics;

	class FRsdkFaceCharacteristic : public BioContracts::IFaceCharacteristics, IEnrollmentAble
	{
	public:
		friend ImageCharacteristics;

		~FRsdkFaceCharacteristic() {}

		void analyze( const FRsdk::Image& image
			          , const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test
			          , FRsdk::ISO_19794_5::TokenFace::Creator& extractor)
		{
			if (eyes_.size() < 1)
				return;

			FRsdk::AnnotatedImage annotatedImage(image, eyes_.front());
			portrait_characteristics_ = new FRsdk::Portrait::Characteristics(analyzer.analyze(annotatedImage));
			iso_compilance_ = new FRsdk::ISO_19794_5::FullFrontal::Compliance(
				iso19794Test.assess(*portrait_characteristics_));


			image_ = new FRsdk::AnnotatedImage(extractor.extract(annotatedImage));
		}

		bool isCompliant()
		{
			return   iso_compilance_->goodExposure()
				&& iso_compilance_->isFrontal()
				&& iso_compilance_->goodVerticalFacePosition();
		}

		bool isCompliantWithBestPractice()
		{
			return   isCompliant() && iso_compilance_->eyesOpenBestPractice();
		}

		FRsdk::Box faceBox() const
		{
			return FRsdk::Portrait::earToEarChinCrownSurroundingBox(*portrait_characteristics_);
		}

		unsigned int size() const
		{
			FRsdk::Box box = faceBox();
			return std::abs((box.originx() - box.endx()) * (box.originy() - box.endy()));
		}

		const FRsdk::Image& annotatedImage() const
		{
			return (*image_).first;
		}


		void saveFace(const std::string& filename)
		{
			std::ofstream out(filename, std::ios::out | std::ios::binary);
			FRsdk::Png::save(annotatedImage(), out); // save png image
		}

		const std::string& firTemplate() const
		{
			return fir_bytestring_;
		}

		void enroll(IEnrollmentProcessor& processor )
		{
			fir_bytestring_ = processor.enroll(annotatedImage());		
		}	

		bool hasEyes() const
		{
			return eyes_.size() > 0;
		}

		unsigned int age() const
		{
			return portrait_characteristics_->age();
		}

		bool isMale() const
		{
			return (portrait_characteristics_->isMale() > 0.5f);
		}

		bool hasFace() const 
		{
			return face_.confidence >FACE_CONFIDENCE_THRESHOLD;
		}

		const FRsdk::Face::Location& faceLocation() const
		{
			return face_;
		}

		const FRsdk::Eyes::LocationSet& eyesLocations() const
		{
			return eyes_;
		}

	private:
		FRsdkFaceCharacteristic(  const FRsdk::Face::Location& face
		                      	, const FRsdk::Eyes::LocationSet& eyes)
		                      	: face_(face), eyes_(eyes)
		{}

	private:
		std::string fir_bytestring_;	
		FRsdk::Face::Location                                          face_;
		FRsdk::Eyes::LocationSet                                       eyes_;		
		FRsdk::CountedPtr<FRsdk::AnnotatedImage>                       image_;
		FRsdk::CountedPtr<FRsdk::Portrait::Characteristics>            portrait_characteristics_;
		FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Compliance> iso_compilance_;

		const float FACE_CONFIDENCE_THRESHOLD = 1.0f;

		
	};
	
	struct FRsdkFaceCharacteristicComparator
	{
		bool operator()( const std::unique_ptr<FRsdkFaceCharacteristic>& first
			             , const std::unique_ptr<FRsdkFaceCharacteristic>& second)
		{
			return (first->size() < second->size());
		}
	};
	

	class ImageCharacteristics : public BioContracts::IImageCharacteristics
	{
	private:
		typedef std::unique_ptr<FRsdkFaceCharacteristic> FaceItemType;

	public:

		ImageCharacteristics() {}
		~ImageCharacteristics()
		{
			clear();
		}

		void push( const FRsdk::Face::Location& face
			       , const FRsdk::Eyes::LocationSet& eyes)
		{						
			faces_.push_back(FaceItemType(new FRsdkFaceCharacteristic(face, eyes)));
		}

		void analyze( const FRsdk::Image& image
			          , const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test
			          , FRsdk::ISO_19794_5::TokenFace::Creator& extractor)
		{
			for (auto it = faces_.begin(); it != faces_.end(); ++it)
				(*it)->analyze(image, analyzer, iso19794Test, extractor);

			FRsdkFaceCharacteristicComparator comparator;
			std::sort(faces_.begin(), faces_.end(), comparator); //from min size to max size
			//faces_.sort(comparator); //from min size to max size
		}
		
		IEnrollmentAble& getEnrollmentAbleFace() const
		{
			return *(faces_.back());
		}

		bool hasFaces() const
		{
			return size() > 0;
		}
		
		size_t size() const { return faces_.size(); }

		const BioContracts::IFaceCharacteristics& operator[](size_t index) const
		{
			if (index >= size())
				throw std::invalid_argument("Index out of range");

			return *(faces_[index]);
		}
				
		void clear() { 
			faces_.clear();
		};
		
		
	public:
		const std::vector<FaceItemType>::const_iterator cbegin() const
		{
			return faces_.cbegin();
		}

		const std::vector<FaceItemType>::const_iterator cend() const
		{
			return faces_.cend();
		}
		
	private:
		ImageCharacteristics(const ImageCharacteristics& that);	

	private:	

		std::vector<std::unique_ptr<FRsdkFaceCharacteristic>> faces_;


	};

}

#endif

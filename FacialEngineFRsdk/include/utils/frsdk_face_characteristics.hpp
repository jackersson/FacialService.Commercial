#ifndef FRsdkFaceCharacteristics_INCLUDED
#define FRsdkFaceCharacteristics_INCLUDED

#include "common/iface_characteristics.hpp"
#include "common/iimage_characteristic.hpp"
#include "biotasks/ienrollment_processor.hpp"
#include "biotasks/iverification_processor.hpp"
#include "biotasks/ienrollment_able.hpp"
#include "biotasks/iverification_able.hpp"
#include "utils/utils.hpp"

#include "utils/face_vacs_includes.hpp"
#include "utils/compliance_iso_characteristics.h"

#include <frsdk/tokenface.h>
#include <frsdk/portrait.h>

namespace BioFacialEngine
{
	class ImageCharacteristics;	

	class FRsdkFaceCharacteristic : public BioContracts::IFaceCharacteristics
		                            , public IEnrollmentAble
		                            , public IVerificationAble
	{
	public:
		friend ImageCharacteristics;
				
		~FRsdkFaceCharacteristic() {}

		void analyze( const FRsdk::Image& image
			          , const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test
			          , FRsdk::ISO_19794_5::TokenFace::Creator& extractor)
		{
			Utils utils;
			id_ = utils.getTicks();

			if (!hasEyes())
				return;

			FRsdk::AnnotatedImage annotatedImage(image, eyes_.front());
			/*portrait_characteristics_ = new FRsdk::Portrait::Characteristics(analyzer.analyze(annotatedImage));
			iso_compilance_ = new FRsdk::ISO_19794_5::FullFrontal::Compliance(
				iso19794Test.assess(*portrait_characteristics_));

			ComplianceIsoCharacteristics compliance;
			iso_compliance_result_        = compliance.analyze(*iso_compilance_);
			iso_compliant_                = compliance.isIsoCompliant(iso_compliance_result_);
			iso_compliant_best_practices_ = compliance.isIsoBestPractices(iso_compliance_result_);
			*/

			image_ = new FRsdk::AnnotatedImage(extractor.extract(annotatedImage));
		}

		long id() const
		{
			return id_;
		}

		bool isCompliant() const
		{
			return iso_compliant_;
		}

		int isoCompliantResult() const
		{
			return iso_compliance_result_;
		}

		bool isCompliantWithBestPractice() const
		{
			return   iso_compliant_best_practices_;
		}

		float eyeDistance() const
		{
			return portrait_characteristics_->eyeDistance();
		}


		float glasses() const
		{
			return portrait_characteristics_->glasses();
		}

		float naturalSkinColor() const
		{
			return portrait_characteristics_->naturalSkinColour();
		}

		float headWidth() const
		{
			return portrait_characteristics_->widthOfHead();
		}

		float headLength() const
		{
			return portrait_characteristics_->lengthOfHead();
		}

		float poseAngleRoll() const
		{
			return portrait_characteristics_->poseAngleRoll();
		}

		float chin() const
		{
			return portrait_characteristics_->chin();
		}

		float crown() const
		{
			return portrait_characteristics_->crown();
		}

		float leftEar() const
		{
			return portrait_characteristics_->ear0();
		}

		float rightEar() const
		{
			return portrait_characteristics_->ear1();
		}

		float confidence() const
		{
			return face_.confidence;
		}

		const BioContracts::Ethnicity ethnicity() const
		{
			FRsdk::Portrait::EthnicityMeasurements eth = portrait_characteristics_->ethnicity();

			if (eth.asian > std::max(eth.black, eth.white))
				return BioContracts::Ethnicity::Asian;
			else if (eth.black > std::max(eth.asian, eth.white))
				return BioContracts::Ethnicity::Black;
	
			return BioContracts::Ethnicity::White;
		}

		float mouthClosed() const
		{
			return portrait_characteristics_->mouthClosed();
		}

		const BioContracts::Position faceCenter() const
		{
			return toPosition(portrait_characteristics_->faceCenter());
		}

		const BioContracts::SurroundingBox faceBox() const
		{
			return toSurroundingBox(FRsdk::Portrait::earToEarChinCrownSurroundingBox(*portrait_characteristics_));
		}

		unsigned int faceSize() const
		{
			BioContracts::SurroundingBox box = faceBox();
			BioContracts::Position origin( box.origin());
			BioContracts::Position end   (  box.end() );

			return (unsigned int)std::abs((origin.x() - end.x()) * (origin.y() - end.y()));
		}

		const FRsdk::Image& annotatedImage() const
		{
			return (*image_).first;
		}

		const FRsdk::FIR& fir() const
		{
			return *fir_;
		}

		bool hasFir() const
		{
			return identification_record_->size() > 0;
		}


		void saveFace(const std::string& filename)
		{
			//std::ofstream out(filename, std::ios::out | std::ios::binary);
			//FRsdk::Png::save(annotatedImage(), out); // save png image
		}

		const std::string& firTemplate() const
		{
			return fir_bytestring_;
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
			return (portrait_characteristics_->isMale() > HALF_PROBABILITY);
		}

		bool hasFace() const 
		{
			return face_.confidence > FACE_CONFIDENCE_THRESHOLD;
		}

		const BioContracts::EyesDetails eyesDetails() const
		{
			return toEyesDetails(eyes_.front());
		}


		void enroll(IEnrollmentProcessor& processor)
		{
			processor.enroll(annotatedImage(), identification_record_);

		
			//test_->swap(*able);
			//fir_ = processor.build(fir_bytestring_);
			//fir_bytestring_.copy(bytes, bytes->size(), 0);
			//std::cout << "test 2 " << fir_->size() << " " << hasFir() << " " << id_ << std::endl;
		}

		float verifyWith( const FRsdk::Image& image
			              , IVerificationProcessor& processor)
		{
			
			if (!hasFace() || !hasEyes())
				return 0.0f;

			//if (fir_bytestring_.size() <= 0)
				//return 0.0f;

			if (identification_record_ == NULL)
				return 0.0f;

			//std::cout << "test 3 "  << id_ << std::endl;
			//std::cout << "test 3 " << fir_->size() << " " << id_ << std::endl;

			if (fir_ == NULL)
				fir_ = processor.build(identification_record_->fir());


		//	std::cout << fir_->size() << std::endl;
			if (!hasFir())
				return 0.0f;
				//fir_ = processor.build(fir_bytestring_);	

			float res = processor.verify(image, *fir_);
		
			return res;
		}

		/*
		const BioContracts::FaceLocation faceLocation() const
		{		
			return toFaceLocation(face_);
		}
		*/

		//const BioContracts::EyeLocation eyesLocation() const
		//{		
		//	return toEyesLocation(eyes_.front());
		//}

	private:
		FRsdkFaceCharacteristic(  const FRsdk::Face::Location& face
		                      	, const FRsdk::Eyes::LocationSet& eyes)
		                      	: face_(face), eyes_(eyes)
														, iso_compliance_result_(0)
														, iso_compliant_(false)
														, iso_compliant_best_practices_(false)
														, fir_bytestring_("")
														, identification_record_(new BioContracts::IdentificationRecord(""))
														
		{}

  private:
		BioContracts::FaceLocation toFaceLocation(const FRsdk::Face::Location& face) const
		{
			BioContracts::FaceLocation face_location(toPosition(face.pos)
				                                      , face.width
				                                      , face.confidence
				                                      , face.rotationAngle);
			return face_location;
		}

		BioContracts::EyesDetails toEyesDetails(const FRsdk::Eyes::Location& eyes) const
		{
			BioContracts::EyeLocation left( toEyesLocation(eyes.first
				                            , eyes.firstConfidence
				                            , portrait_characteristics_->eye0GazeFrontal()
				                            , portrait_characteristics_->eye0Open()  > HALF_PROBABILITY
				                            , portrait_characteristics_->eye0Red()   > HALF_PROBABILITY
				                            , portrait_characteristics_->eye0Tinted()> HALF_PROBABILITY	));

			BioContracts::EyeLocation right( toEyesLocation(eyes.second
				                             , eyes.secondConfidence
				                             , portrait_characteristics_->eye1GazeFrontal()
				                             , portrait_characteristics_->eye1Open()  > HALF_PROBABILITY
				                             , portrait_characteristics_->eye1Red()   > HALF_PROBABILITY
				                             , portrait_characteristics_->eye1Tinted()> HALF_PROBABILITY));

		  BioContracts::EyesDetails details(left, right);
		  return details;
		}

		BioContracts::EyeLocation toEyesLocation( const FRsdk::Position& pos
			                                      , float confidence   = 0.0f
			                                      , float gaze_frontal = 0.0f
			                                      , bool  is_open      = false
			                                      , bool  is_red       = false
			                                      , bool  is_tined     = false ) const
		{
			
			BioContracts::EyeLocation eyes_location(  toPosition(pos), confidence																						
																							, is_open	, is_red, is_tined, gaze_frontal );
			return eyes_location;
		}
		

		BioContracts::Position toPosition(const FRsdk::Position& pos) const 
		{
			BioContracts::Position position(pos.x(), pos.y());
			return position;
		}

		BioContracts::SurroundingBox toSurroundingBox(const FRsdk::Box& box) const
		{			
			BioContracts::SurroundingBox sbox(box.originx(), box.originy(), box.endx(), box.endy());
			return sbox;
		}

	private:
		int  iso_compliance_result_       ;
		bool iso_compliant_               ;
		bool iso_compliant_best_practices_;

		long id_;

		std::string fir_bytestring_;	
		FRsdk::Face::Location                                          face_                    ;
		FRsdk::Eyes::LocationSet                                       eyes_                    ;		
		FRsdk::CountedPtr<FRsdk::AnnotatedImage>                       image_                   ;
		FRsdk::CountedPtr<FRsdk::Portrait::Characteristics>            portrait_characteristics_;
		FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Compliance> iso_compilance_          ;
		FRsdk::CountedPtr<FRsdk::FIR>                                  fir_;

		BioContracts::IdentificationRecordRef identification_record_;;

		const float FACE_CONFIDENCE_THRESHOLD = 1.0f;
		const float HALF_PROBABILITY          = 0.5f;

		
	};
	
	struct FRsdkFaceCharacteristicComparator
	{
		bool operator()( const std::unique_ptr<FRsdkFaceCharacteristic>& first
			             , const std::unique_ptr<FRsdkFaceCharacteristic>& second)
		{
			return (first->faceSize() < second->faceSize());
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
		
		FRsdkFaceCharacteristic& getEnrollmentAbleFace() const
		{
			return *(faces_.back());
		}

		bool hasFaces() const
		{
			return size() > 0;
		}
		
		size_t size() const { return faces_.size(); }

		FRsdkFaceCharacteristic& operator[](size_t index) const
		{
			if (index >= size())
				throw std::invalid_argument("Index out of range");

			return *(faces_[index]);
		}
				
		void clear() { 
			faces_.clear();
		};
		
		/*
	public:
		const std::vector<std::unique_ptr<FRsdkFaceCharacteristic>>::iterator cbegin() const
		{
			return faces_.begin();
		}

		const std::vector<std::unique_ptr<FRsdkFaceCharacteristic>>::iterator cend() const
		{
			return faces_.end();
		}
		*/
		
	private:
		ImageCharacteristics(const ImageCharacteristics& that);	

	private:	

		std::vector<std::unique_ptr<FRsdkFaceCharacteristic>> faces_;


	};

	typedef std::shared_ptr<ImageCharacteristics> ImageCharacteristicsType;
}

#endif

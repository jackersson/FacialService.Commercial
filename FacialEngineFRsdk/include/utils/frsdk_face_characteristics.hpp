#ifndef FRsdkFaceCharacteristics_INCLUDED
#define FRsdkFaceCharacteristics_INCLUDED


#include "utils/face_vacs_convertion.hpp"

#include "common/iface_characteristics.hpp"
#include "common/iimage_characteristic.hpp"
#include "biotasks/ienrollment_processor.hpp"
#include "biotasks/iverification_processor.hpp"
#include "biotasks/ienrollment_able.hpp"
#include "biotasks/iverification_able.hpp"
#include "utils/utils.hpp"

#include "utils/compliance_iso_characteristics.h"

#include <frsdk/tokenface.h>


#include <frsdk\bmp.h>

#include <fstream>

#include <Windows.h>
#include <ppl.h>

namespace BioFacialEngine
{
	class ImageCharacteristics;	
	//class FacialAcquisition   ;

	class FRsdkIsoCompliance : public BioContracts::IComlianceIsoTemplate
	{
	public:
		~FRsdkIsoCompliance() {}

		FRsdkIsoCompliance(const FRsdkIsoCompliance&) {}
		FRsdkIsoCompliance& operator=(const FRsdkIsoCompliance&) {}
				
		bool isoCompatible()  const	{
			return iso_compatible_;
		}

		int  isoTemplate() const	{
			return iso_template_;
		}

		bool bestPracticeCompatible() const {
			return iso_compatible_best_practices_;
		}

	//private:
		FRsdkIsoCompliance( const FRsdk::ISO_19794_5::FullFrontal::Compliance& iso_compliance)
		                	: iso_template_(0), iso_compatible_(false), iso_compatible_best_practices_(false)
		{
			ComplianceIsoCharacteristics compliance;
			iso_template_ = compliance.analyze(iso_compliance);
			iso_compatible_ = compliance.isIsoCompliant(iso_template_);
			iso_compatible_best_practices_ = compliance.isIsoBestPractices(iso_template_);
		}

	private:
		int  iso_template_;
		bool iso_compatible_;
		bool iso_compatible_best_practices_;

	};

	typedef std::shared_ptr<FRsdkIsoCompliance> FRsdkIsoCompliancePtr;


	class FRsdkFaceCharacteristic : public BioContracts::IFaceCharacteristics
	{
	public:
		FRsdkFaceCharacteristic( const FRsdk::Portrait::Characteristics& portrait)
		                       : portrait_characteristics_(portrait)
		{
			//unsigned int start = clock();
			//portrait.isMale();
			//portrait.age();
			/*
			concurrency::parallel_invoke(
				//[&]() {				std::cout << "age : " << portrait.age() << std::endl; },
				[&]() {				std::cout << "male : " << portrait.isMale() << std::endl; },
			[&]() {				std::cout << "male : " << portrait.isMale() << std::endl; }
			);
			*/
		
			//std::cout << "pr age" << clock() - start << std::endl;
		}

		~FRsdkFaceCharacteristic() {}

		float eyeDistance() const	{
			return portrait_characteristics_.eyeDistance();
		}

		float glasses() const	{
			return portrait_characteristics_.glasses();
		}

		float naturalSkinColor() const	{
			return portrait_characteristics_.naturalSkinColour();
		}

		float headWidth() const {
			return portrait_characteristics_.widthOfHead();
		}

		float headLength() const{
			return portrait_characteristics_.lengthOfHead();
		}

		float poseAngleRoll() const{
			return portrait_characteristics_.poseAngleRoll();
		}

		float chin() const{
			return portrait_characteristics_.chin();
		}

		float crown() const	{
			return portrait_characteristics_.crown();
		}

		float leftEar() const	{
			return portrait_characteristics_.ear0();
		}

		float rightEar() const	{
			return portrait_characteristics_.ear1();
		}

		const BioContracts::Ethnicity ethnicity() const	{
			FRsdk::Portrait::EthnicityMeasurements eth = portrait_characteristics_.ethnicity();

			if (eth.asian > std::max(eth.black, eth.white))
				return BioContracts::Ethnicity::Asian;
			else if (eth.black > std::max(eth.asian, eth.white))
				return BioContracts::Ethnicity::Black;

			return BioContracts::Ethnicity::White;
		}

		float mouthClosed() const	{
			return portrait_characteristics_.mouthClosed();
		}

		const BioContracts::Position faceCenter() const	{
			return converter_.toPosition(portrait_characteristics_.faceCenter());
		}

		const BioContracts::SurroundingBox faceBox() const	{
			return converter_.toSurroundingBox(FRsdk::Portrait::earToEarChinCrownSurroundingBox(portrait_characteristics_));
		}

		unsigned int faceSize() const	{
			BioContracts::SurroundingBox box = faceBox();
			BioContracts::Position origin(box.origin());
			BioContracts::Position end(box.end());

			return (unsigned int)std::abs((origin.x() - end.x()) * (origin.y() - end.y()));
		}

		unsigned int age() const	{
			return portrait_characteristics_.age();
		}

		bool isMale() const	{
			return (portrait_characteristics_.isMale() > 0.5f);
		}

		const FRsdk::Portrait::Characteristics& portraitCharacteristics() const	{
			return portrait_characteristics_;
		}		

		/*
		void operator=(FRsdk::Portrait::Characteristics const& value)	{
			portrait_characteristics_ = value;
		}
		*/
		/*
		public implicit operator FRsdkFaceCharacteristic(double value)
		{
			return new Velocity(value);
		}
		*/


	private:
		//FRsdkFaceCharacteristic( const FRsdk::Portrait::Characteristics& portrait)
		//                       : portrait_characteristics_(portrait)
		//{}

		//FRsdkFaceCharacteristic const & operator=(FRsdkFaceCharacteristic const&);

	private:
		FRsdk::Portrait::Characteristics  portrait_characteristics_;


		FaceVacsConvertion converter_;

		//const float HALF_PROBABILITY          = 0.5f;
	};

	class FRsdkFace : public BioContracts::IFace
	{
	public:
		friend ImageCharacteristics;

		~FRsdkFace() {}		

		float faceWidth() const	{
			return face_.width;
		}

		bool hasEyes() const {
			return eyes_.size() > 0;
		}
		
		bool hasFace() const	{
			return face_.confidence > FACE_CONFIDENCE_THRESHOLD;
		}

		bool hasFIR() const	{
			return fir_ != NULL;
		}

		bool hasFacialImage() const
		{
			return (facial_image_ != NULL);
		}

		bool canBuildFIR() const	{
			return identification_record_->size() > 0;
		}

		float confidence() const {
			return face_.confidence;
		}

		const FRsdk::Eyes::LocationSet& eyes() const	{
			return eyes_;
		}

		const FRsdk::Face::Location& face() const	{
			return face_;
		}

		const FRsdk::FIR& fir() const	{
			return *fir_;
		}

		const BioContracts::IComlianceIsoTemplate& isoCompliance() const	{
			return *iso_compliance_;
		}

		const BioContracts::IFaceCharacteristics& characteristics() const	{
			return *face_characteristics_;
		}

		BioContracts::IdentificationRecordRef identificationRecord() {
			return identification_record_;
		}

	private:
		FRsdkFace( const FRsdk::Face::Location& face
		         , const FRsdk::Eyes::LocationSet& eyes )
						 : face_(face)
						 , eyes_(eyes) 						
						 , identification_record_(new BioContracts::IdentificationRecord(""))
		{}


		void setFaceCharacteristics(const FRsdkFaceCharacteristic& face_characteristics) {
			(*face_characteristics_) = face_characteristics;
		}

		void setIsoComplianceResult(const FRsdkIsoCompliance& iso_compliance ) {
			(*iso_compliance_) = iso_compliance;
		}

		void setFir(const FRsdk::FIR& fir)	{
			(*fir_) = fir;
		}
			


	private:
		FRsdk::Face::Location                                          face_;
		FRsdk::Eyes::LocationSet                                       eyes_; 

		FRsdk::CountedPtr<FRsdk::AnnotatedImage>                       facial_image_;

		BioContracts::IdentificationRecordRef                          identification_record_;
		FRsdk::CountedPtr<FRsdk::FIR>                                  fir_;

		std::shared_ptr<FRsdkIsoCompliance>      iso_compliance_      ;
		std::shared_ptr<FRsdkFaceCharacteristic> face_characteristics_;

		const float FACE_CONFIDENCE_THRESHOLD = 1.0f;

	};

	

	/*

	class BioFace : public BioContracts::IFaceCharacteristics
		                            , public IEnrollmentAble
		                            , public IVerificationAble
	{
	public:
		friend ImageCharacteristics;
				
		~BioFace() {}

		void generateID()
		{
			if (id_ > 0)
				return;

			Utils utils;
			id_ = utils.getTicks();
		}

		void acquire( const FRsdk::Image& image
			          , const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test
			          , FRsdk::ISO_19794_5::TokenFace::Creator& extractor	)
		{
			generateID();

			/*
			if (!hasEyes())
				return;
						
			FRsdk::AnnotatedImage annotatedImage(image, eyes_.front());	
			
			concurrency::parallel_invoke (
				[&]() {	extract(annotatedImage, extractor);              },
				[&]() { acquire(annotatedImage, analyzer, iso19794Test); }
			);		
			
		}


		//make private
		void extract(const FRsdk::Image& image, FRsdk::ISO_19794_5::TokenFace::Creator& extractor)
		{
			/*
			if (!hasEyes() || hasFacialImage())
				return;

			FRsdk::AnnotatedImage annotatedImage(image, eyes_.front());

			extract(annotatedImage, extractor);
		
		}

		void extract(const FRsdk::AnnotatedImage& annotatedImage, FRsdk::ISO_19794_5::TokenFace::Creator& extractor)
		{			
			//if (!hasFacialImage())
				//image_ = new FRsdk::AnnotatedImage(extractor.extract(annotatedImage));
		}

		//without extraction
		void acquire( const FRsdk::Image& image
			          , const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test )
		{
			/*
			generateID();

			if (!hasEyes())
				return;

			FRsdk::AnnotatedImage annotatedImage(image, eyes_.front());

			portrait_characteristics_ = new FRsdk::Portrait::Characteristics(analyzer.analyze(annotatedImage));	
		
		}

		//make private
		void acquire( const FRsdk::AnnotatedImage& annotatedImage
			          , const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test)
		{			
			/*portrait_characteristics_ = new FRsdk::Portrait::Characteristics(analyzer.analyze(annotatedImage));
			iso_compilance_ = new FRsdk::ISO_19794_5::FullFrontal::Compliance(
				iso19794Test.assess(*portrait_characteristics_)); 
		
		}

  #pragma region fields
		long id() const
		{
			return id_;
		}

		/*
		const BioContracts::EyesDetails eyesDetails() const
		{
			return toEyesDetails(eyes_.front());
		}
	

#pragma endregion fields
		void enroll(IEnrollmentProcessor& processor)
		{
			//if (!hasFacialImage())
				//return;

			unsigned int start = clock();

			//processor.enroll(annotatedImage(), identification_record_);
			std::cout << " template creation  = " << clock() - start << std::endl;		
		}
		
		float verifyWith( const FRsdk::Image& image
			              , IVerificationProcessor& processor)
		{
			
			//if (!hasFace() || !hasEyes())
			//	return 0.0f;

			//if (fir_bytestring_.size() <= 0)
				//return 0.0f;

			//if (identification_record_ == NULL)
			//	return 0.0f;

			//std::cout << "test 3 "  << id_ << std::endl;
			//std::cout << "test 3 " << fir_->size() << " " << id_ << std::endl;

			//if (fir_ == NULL)
				//fir_ = processor.build(identification_record_->fir());


		//	std::cout << fir_->size() << std::endl;
			//if (!hasFir())
			//	return 0.0f;
				//fir_ = processor.build(fir_bytestring_);	

			//float res = processor.verify(image, *fir_);
			return NULL;
			//return res;
		}

		/*
		const BioContracts::FaceLocation faceLocation() const
		{		
			return toFaceLocation(face_);
		}
		

		//const BioContracts::EyeLocation eyesLocation() const
		//{		
		//	return toEyesLocation(eyes_.front());
		//}

	private:
		//BioFace(), identification_record_(new BioContracts::IdentificationRecord(""))
														
		//{}

  private:
	

	private:
		
		long id_;

		//std::string fir_bytestring_;	
		//FRsdk::Face::Location                                          face_                    ;
		//FRsdk::Eyes::LocationSet                                       eyes_                    ;		
		//FRsdk::CountedPtr<FRsdk::AnnotatedImage>                       image_                   ;
		//FRsdk::CountedPtr<FRsdk::Portrait::Characteristics>            portrait_characteristics_;
		//FRsdk::CountedPtr<FRsdk::ISO_19794_5::FullFrontal::Compliance> iso_compilance_          ;
		//FRsdk::CountedPtr<FRsdk::FIR>                                  fir_;

		//BioContracts::IdentificationRecordRef identification_record_;;

		//const float FACE_CONFIDENCE_THRESHOLD = 1.0f;
		//const float HALF_PROBABILITY          = 0.5f;

		
	};
	*/
	typedef std::shared_ptr<FRsdkFace> FaceItemType;
	struct FRsdkFaceCharacteristicComparator
	{
		bool operator()( const FaceItemType& first
		             	 , const FaceItemType& second)
		{
			return (first->faceWidth() < second->faceWidth());
		}
	};
	

	class ImageCharacteristics : public BioContracts::IImageCharacteristics
	{		
	public:

		ImageCharacteristics(FaceVacsImage image) : image_(image){}
		~ImageCharacteristics()
		{
			clear();
		}

		void push( const FRsdk::Face::Location& face
			       , const FRsdk::Eyes::LocationSet& eyes
						  )
		{						
			faces_.push_back(FaceItemType(new FRsdkFace(face, eyes)));
		}

		void acquire( const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test ) 
		{
			concurrency::parallel_for_each( faces_.cbegin(), faces_.cend(),
				                             [&](FaceItemType face)
			{			
				//face->acquire(*image_, analyzer, iso19794Test);
			});
		}

		void acquire( const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test
			          , FRsdk::ISO_19794_5::TokenFace::Creator& extractor) 
		{
			concurrency::parallel_for_each( faces_.cbegin(), faces_.cend(),
				                             [&](FaceItemType face)
			{
				//face->acquire(*image_, analyzer, iso19794Test, extractor);
			});
		}

		void enroll( const FRsdk::Portrait::Analyzer& analyzer
			         , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test
			         , FRsdk::ISO_19794_5::TokenFace::Creator& extractor 
			         , IEnrollmentProcessor& processor                   )
		{
			concurrency::parallel_for_each(   faces_.cbegin(), faces_.cend(),
				                             [&](FaceItemType face)
			{				
				//face->extract(*image_, extractor);

				//concurrency::parallel_invoke(
				//	[&]() {face->acquire(*image_, analyzer, iso19794Test); },
				//	[&]() {face->enroll(processor); }
			//	);

			});
		}

		void verify(ImageCharacteristics& image, IVerificationProcessor& processor)
		{
			
			concurrency::parallel_for_each( faces_.cbegin(), faces_.cend(),
				                             [&](FaceItemType first)
			{
				concurrency::parallel_for_each(image.cbegin(), image.cend(),
					                             [&](FaceItemType second )
				{
					//first->verifyWith(second->annotatedImage(), processor);
				});
			});
			
		}

		void analyze( const FRsdk::Portrait::Analyzer& analyzer
			          , const FRsdk::ISO_19794_5::FullFrontal::Test& iso19794Test
			          , FRsdk::ISO_19794_5::TokenFace::Creator& extractor)
		{
			concurrency::parallel_for_each( faces_.cbegin(), faces_.cend(),
				                             [&](FaceItemType face)
			{
				//face->acquire(*image_, analyzer, iso19794Test, extractor);
			});

			
			FRsdkFaceCharacteristicComparator comparator;
			std::sort(faces_.begin(), faces_.end(), comparator); //from min size to max size			
		}
		
		FRsdkFace& getFrontFace() const
		{
			return *(faces_.back());
		}

		bool hasFaces() const
		{
			return size() > 0;
		}
		
		size_t size() const { return faces_.size(); }

		FRsdkFace& operator[](size_t index) const
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
			return faces_.begin();
		}

		const std::vector<FaceItemType>::const_iterator cend() const
		{
			return faces_.end();
		}		
		
	private:
		ImageCharacteristics(const ImageCharacteristics& that);	

	private:	
		FaceVacsImage             image_;
		std::vector<FaceItemType> faces_;


	};

	typedef std::shared_ptr<ImageCharacteristics> ImageCharacteristicsType;
}

#endif

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

  class FRsdkPositionW : public BioContracts::IPosition
  {
  public:
  	FRsdkPositionW(const FRsdk::Position& position) : position_(position.x(), position.y()) {}
		FRsdkPositionW(float xx = 0, float yy = 0) : position_(xx, yy) {}

		void copyFrom(const FRsdk::Position& position)
		{
			position_.first = position.x();
			position_.second = position.y();
		}

		void copyFrom(float xx, float yy)
		{
			position_.first =  xx;
			position_.second = yy;
		}
  
  	float x() const {
  		return position_.first;
  	}
  
  	float y() const {
  		return position_.second;
  	}
    
  private:
		std::pair<float, float> position_;
  };
  
	class FRsdkFaceW : public BioContracts::IFace
  {
  public:
  	FRsdkFaceW(const FRsdk::Face::Location& location) : face_(location) {
  		std::cout << "face : " << location.pos.x() << " " << location.pos.y() << std::endl;
  	}
  
  	float confidence() const {
  		return face_.confidence;
  	}
  
  	float width()   const {
  		return face_.width;
  	}
  
  	float rotationAngle() const {
  		return face_.rotationAngle;
  	}

	   
  	const FRsdk::Face::Location& instance() const {
  		return face_;
  	}
  
  private:
  	FRsdk::Face::Location face_;
  };
  
  
	class FRsdkEye : public BioContracts::IEye
  {
  public:
  	FRsdkEye(const FRsdk::Position& pos, float confidence) : position_(pos), confidence_(confidence) {
  		std::cout << "eyes : " << pos.x() << " " << pos.y() << std::endl;
  	}
  
  	const FRsdkPositionW& position() const	{
  		return position_;
  	}
  
  	float confidence() const {
  		return confidence_;
  	}
  private:
  	float confidence_;
  	FRsdkPositionW position_;
  };
  
	class FRsdkEyesW : public BioContracts::IEyes
  {
  public:
  	FRsdkEyesW(  const FRsdk::Eyes::Location& location) 
  		         : eyes_  (location)
  		         , eyes_w_( FRsdkEye(location.first , location.firstConfidence )
  		         , FRsdkEye(location.second, location.secondConfidence))
  	{}
  
		const BioContracts::IEye& left() const {
  		return eyes_w_.first;
  	}
  
		const BioContracts::IEye& right() const {
  		return eyes_w_.second;
  	}
  		
  	const FRsdk::Eyes::Location& instance() const {
  		return eyes_;
  	}
  private:
  	FRsdk::Eyes::Location eyes_;
  	std::pair<FRsdkEye, FRsdkEye> eyes_w_;
  };
  
	class FRsdkBoxW : public BioContracts::IBox
  {
  public:
  	FRsdkBoxW( const FRsdk::Box& box)
  		       : origin_((float)box.originx(), (float)box.originy())
						 , end_   ((float)box.endx()   , (float)box.endy())
  	{}

		FRsdkBoxW(float originx = 0, float originy = 0, float endx = 0, float endy = 0)
		        	: origin_(originx, originy)
							, end_(endx, endy)
  	{}

		void copyFrom(const FRsdk::Box& box)
		{		
			origin_.copyFrom(box.originx(), box.originy());
			end_   .copyFrom(box.endx(), box.endy());		
		}
  
		const BioContracts::IPosition& origin() const {
  		return origin_;
  	}
  
		const BioContracts::IPosition& end() const {
  		return end_;
  	}
		
		const unsigned int size() const {
			return (unsigned int ) std::abs((origin_.x() - end_.x()) * (origin_.y() - end_.y()));
		}  	
  		
  	
  private:
  	
  	FRsdkPositionW origin_;
		FRsdkPositionW end_;
  };


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
													 , face_center_()
													 , bounding_box_()
		{			

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

		const BioContracts::IPosition& faceCenter() const	{
			face_center_.copyFrom(portrait_characteristics_.faceCenter());
			return face_center_;
		}

		const BioContracts::IBox& faceBox() const	{
			bounding_box_.copyFrom(FRsdk::Portrait::earToEarChinCrownSurroundingBox(portrait_characteristics_));
			return bounding_box_;
		}

		unsigned int faceSize() const	{		
			return faceBox().size();
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

	private:
		//FRsdkFaceCharacteristic( const FRsdk::Portrait::Characteristics& portrait)
		//                       : portrait_characteristics_(portrait)
		//{}

		//FRsdkFaceCharacteristic const & operator=(FRsdkFaceCharacteristic const&);

	private:
		FRsdk::Portrait::Characteristics  portrait_characteristics_;

		mutable FRsdkPositionW face_center_ ;
		mutable FRsdkBoxW      bounding_box_;
		//FaceVacsConvertion converter_;

		//const float HALF_PROBABILITY          = 0.5f;
	};
	
}


#endif

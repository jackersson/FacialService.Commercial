#ifndef FaceInfo_INCLUDED
#define FaceInfo_INCLUDED

#include "feedback/facial_enrollment_feedback.hpp"
#include "utils/face_vacs_includes.hpp"
#include "wrappers/frsdk_face_characteristics.hpp"
#include "common/iimage_characteristic.hpp"

namespace FRsdkEntities
{
	class FaceInfo : public BioContracts::IFaceInfo
  {
  public:
  	FaceInfo( const FRsdk::Face::Location& faceLocation
  		      , const FRsdk::Eyes::Location& eyes
  					, const FRsdkTypes::FaceVacsImage image)
  					:	face_ (faceLocation)
  					, eyes_ (eyes )
  					, parent_image_(image)
  					, extracted_image_(nullptr)
  					, fir_(new FacialFeedback::FacialEnrollmentFeedback())
  					, portrait_characteristics_(nullptr)
  					, iso_compliance_(nullptr)
  	{		
  		id_ = static_cast<int>(faceLocation.pos.x() +  faceLocation.pos.y());
  	}
  
  	int id() const { return id_; }
  
  	std::string image_id() const	{
  		return parent_image_->name();
  	}
  
  	bool has_fir() const	{
  		return fir_->good();
  	}
  	
  	bool has_extracted_image() const {
  		return (extracted_image_ != nullptr);
  	}
  		
  	FRsdk::AnnotatedImage annotated_image() const	{
  		return FRsdk::AnnotatedImage(*parent_image_, eyes_.instance());
  	}
  
  	const FRsdkFaceCharacteristic& characteristics() const override	{
  		return *portrait_characteristics_;
  	}

		const FRsdkIsoCompliance& iso_compliance() const override {
			return *iso_compliance_;
  	}

		const BioContracts::IFace& face() const override	{
			return face_;
  	}

		const BioContracts::IEyes& eyes()	const override {

			if (portrait_characteristics_ != nullptr)	{
				portrait_characteristics_->set_eyes_characteristics(eyes_);			
			}
			return eyes_;
		}

		std::string identification_record() const override	{
			return enrollment_data()->bytestring();
		}
  
  	FRsdkTypes::FaceVacsImage extracted_image() const	{
  		return extracted_image_;
  	}
  
  	void set_facial_image(FRsdkTypes::FaceVacsImage token) {
  		extracted_image_ = token;
  	}
  		
  	FacialFeedback::FacialEnrollmentFeedbackPtr enrollment_data() const {	
  		return fir_;
  	}
  
  	void set_portrait_characteristics(FRsdkFaceCharacteristicPtr portrait) {
  		portrait_characteristics_ = portrait;
  	}
  
  	void set_iso_compliance(FRsdkIsoCompliancePtr iso_compliance ) {
  		iso_compliance_ = iso_compliance;
  	}
  
  private:
  	FaceInfo(const FaceInfo& rhs);
  	FaceInfo const & operator=(FaceInfo const&);	
  
  	int id_;
  
  	FRsdkFaceW          face_ ;
  	mutable FRsdkEyesW  eyes_ ;

  	FRsdkTypes::FaceVacsImage     parent_image_   ;
  	FRsdkTypes::FaceVacsImage     extracted_image_;
  
  	FacialFeedback::FacialEnrollmentFeedbackPtr fir_;
  
  	FRsdkFaceCharacteristicPtr portrait_characteristics_;
  	FRsdkIsoCompliancePtr      iso_compliance_          ;  	
  };

  typedef std::shared_ptr<FaceInfo> FaceInfoPtr;
	typedef std::pair<FRsdk::Face::Location, FRsdk::Eyes::Location>  FaceVacsFullFace;
}
#endif
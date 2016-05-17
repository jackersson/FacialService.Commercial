#ifndef FacialEnrollment_INCLUDED
#define FacialEnrollment_INCLUDED

#include "biotasks/ienrollment_processor.hpp"
#include "biotasks/ienrollment_able.hpp"
#include "utils/face_vacs_includes.hpp"
#include "feedback/facial_enrollment_feedback.hpp"

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Enrollment::Processor> FRsdkEnrollmentProcessor;	

	class FacialEnrollment : public IEnrollmentProcessor
	{
	public:
		FacialEnrollment(const std::string& configuration_filename);
		FacialEnrollment(FaceVacsConfiguration configuration);
		~FacialEnrollment(){}
		

		void enroll( IEnrollmentAble& enrollment_able );
		void enroll( const std::string& filename      );

		BioContracts::IdentificationRecordRef enroll(const FRsdk::Image& image);

		void enroll( const FRsdk::Image& image
			         , BioContracts::IdentificationRecordRef fir);

		FRsdkFir build(const std::string& bytes);

	private:
		bool init(FaceVacsConfiguration configuration);

	private:
		FRsdkEnrollmentProcessor processor_  ;
		FirBuilderRef            fir_builder_;
	private:
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};
}

#endif
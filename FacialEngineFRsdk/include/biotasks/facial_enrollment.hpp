#ifndef FacialEnrollment_INCLUDED
#define FacialEnrollment_INCLUDED

#include "engine/ifacial_engine.hpp"
#include "biotasks/ienrollment_processor.hpp"
#include "biotasks/facial_acquisition.hpp"
#include "feedback/facial_enrollment_feedback.hpp"

#include <frsdk/enroll.h>

namespace BioFacialEngine
{
	typedef FRsdk::CountedPtr<FRsdk::Enrollment::Processor> FRsdkEnrollmentProcessor;
	
	class FacialEnrollment : public IEnrollmentProcessor
	{

	public:
		FacialEnrollment(const std::string& configuration_filename);
		FacialEnrollment(FaceVacsConfiguration configuration);
		~FacialEnrollment(){}
		

		void enroll( IEnrollmentAble& face_characteristic);
		void enroll(const std::string& filename);

		std::string enroll(const FRsdk::Image& image);

	private:
		bool init(FaceVacsConfiguration configuration);

	private:
		FRsdkEnrollmentProcessor processor_;

	private:
		const float MIN_EYE_DISTANCE = 0.1f;
		const float MAX_EYE_DISTANCE = 0.4f;
	};
}

#endif
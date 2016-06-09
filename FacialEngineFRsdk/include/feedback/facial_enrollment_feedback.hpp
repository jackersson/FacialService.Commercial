#ifndef FacialEnrollmentFeedback_INCLUDED
#define FacialEnrollmentFeedback_INCLUDED

#include <frsdk/enroll.h>
#include "utils/face_vacs_includes.hpp"
#include <memory>

namespace FacialFeedback
{
	typedef std::shared_ptr<FRsdk::FIR> FirPtr;
	

	class FacialEnrollmentFeedback : public FRsdk::Enrollment::FeedbackBody
	{
	public:
		FacialEnrollmentFeedback() : fir_(nullptr), success_(false){}
		virtual ~FacialEnrollmentFeedback() {}

		void start() override {
			fir_     = nullptr;
			success_ = false  ;
		}
		void end() override {}

		void processingImage(const FRsdk::Image& frame) override {}

		void eyesFound(const FRsdk::Eyes::Location& eyes) override{}
		void eyesNotFound() override {}

		void success(const FRsdk::FIR& fir) override
		{						
			fir_ = std::make_shared<FRsdk::FIR>(fir);
			success_ = true;		
		}

		void failure() override {
			success_ = false;
		}

		FirPtr fir() const {		
			return fir_;
		}

		bool good() const {
			return success_;
		}

		std::string bytestring() const	
		{
			std::ostringstream fir_bytestring_stream;
			fir_bytestring_stream << *fir_;
			return fir_bytestring_stream.str();
		}
		
	private:		
		FirPtr fir_;		
		bool success_;	
	};

	typedef FRsdk::CountedPtr<FacialEnrollmentFeedback> FacialEnrollmentFeedbackPtr;
}
#endif
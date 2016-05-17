#ifndef FacialEnrollmentFeedback_INCLUDED
#define FacialEnrollmentFeedback_INCLUDED

#include <frsdk/enroll.h>


namespace BioFacialEngine
{
	class FacialEnrollmentFeedback : public FRsdk::Enrollment::FeedbackBody
	{
	public:
		FacialEnrollmentFeedback( BioContracts::IdentificationRecordRef fir) : fir_(fir) {}
		~FacialEnrollmentFeedback() {}

		void start() {}
		void end() {}

		void processingImage(const FRsdk::Image& frame){}

		void eyesFound(const FRsdk::Eyes::Location& eyes){}
		void eyesNotFound() {}

		void success(const FRsdk::FIR& fir)
		{			
			std::ostringstream fir_bytestring_stream;
			fir_bytestring_stream << fir;
			fir_->copyFrom(fir_bytestring_stream.str());// = new BioContracts::Testable(fir.size());
			//std::ostringstream fir_bytestring_stream;
			//fir_bytestring_stream << fir;
			//fir_bytestring_ = fir_bytestring_stream.str();	
			//fir_bytestring_ = std::make_shared<unsigned char>(new unsigned char[fir.size()]);
			//fir.serialize(fir_bytestring_.get());

			//std::string cfg_path = "C:\\FVSDK_8_9_5\\etc\\frsdk.cfg";
			//FRsdk::Configuration config(cfg_path);
			//FRsdk::FIRBuilder fb(config);
			//std::istringstream istr(fir_bytestring_);
			//fb.build(istr);

			std::cout << "test "  << std::endl;
		}

		void failure(){}

		
	private:
		FacialEnrollmentFeedback(const FacialEnrollmentFeedback&);
		void operator=(const FacialEnrollmentFeedback&);

	private:
		BioContracts::IdentificationRecordRef fir_;
		
	};
}
#endif
#ifndef FacialIdentificationFeedback_INCLUDED
#define FacialIdentificationFeedback_INCLUDED

#include <frsdk/ident.h>
#include <common/matches.hpp>

namespace BioFacialEngine
{
	class FacialIdentificationFeedback : public FRsdk::Identification::FeedbackBody
	{
	public:
		explicit FacialIdentificationFeedback() : success_(false) {}
		virtual ~FacialIdentificationFeedback() {}

		void start() override{
			success_ = false;
		}
		void end() override {}

		void processingImage(const FRsdk::Image& frame) override{}

		void eyesFound(const FRsdk::Eyes::Location& eyes) override{}
		void eyesNotFound() override {}

		void matches(const FRsdk::Matches& matches) override	{
		
			for (auto it = matches.begin(); it != matches.end(); ++it)
			{
				auto current_match = *it;
				long current_face_id = atoi(current_match.first.c_str());
				BioContracts::Match match(current_face_id, current_match.second);
				mathces_.push_back(match);
			}

			success_ = true;
		}
		
		const BioContracts::MatchSet& result() const{
			return mathces_;
		}

	private:
		FacialIdentificationFeedback(const FacialIdentificationFeedback&);
		void operator=(const FacialIdentificationFeedback&);

		bool  success_;
		std::string face_unique_id_;
		BioContracts::MatchSet mathces_;



	};
}
#endif
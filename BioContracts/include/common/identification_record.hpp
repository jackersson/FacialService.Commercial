#ifndef IdentificationRecord_INCLUDED
#define IdentificationRecord_INCLUDED

#include <memory>
#include <string>

namespace BioContracts
{
	
	class IdentificationRecord
	{
	public:
		IdentificationRecord(const IdentificationRecord& testable) : fir_(testable.fir_)
		{}

		IdentificationRecord(const std::string& size) : fir_(size) {}

		void copyFrom(const std::string& fir)  { fir_ = fir; }
		void copyFrom(const IdentificationRecord& testable) { fir_ = testable.fir(); }

		const std::string& fir() const {
			return fir_;
		}

		int size() const {
			return fir_.size();
		}

	private:
		std::string fir_;
	};

	typedef std::shared_ptr<IdentificationRecord> IdentificationRecordRef;
}

#endif
#ifndef IFirBuilder_INCLUDED
#define IFirBuilder_INCLUDED

#include "common\matches.hpp"
#include "utils\face_vacs_includes.hpp"

#include <frsdk\fir.h>

#include <string>
#include <memory>
#include <sstream>

namespace BioFacialEngine
{
	typedef std::shared_ptr<FRsdk::FIR>            FRsdkFir;
	typedef FRsdk::CountedPtr<FRsdk::FIRBuilder >  FRsdkFirBuilder;
	class IFirBuilder
	{
	public:
		virtual ~IFirBuilder() {}

		virtual FRsdkFir build(const std::string& bytes) = 0;
	};

	class FirBilder : public IFirBuilder
	{
	public:
		/*
		FirBilder(const std::string& configuration_filename)
		{
			bool is_ok = false;
			try
			{
				FaceVacsConfiguration configuration = new FRsdk::Configuration(configuration_filename);
				is_ok = init(configuration);
			}
			catch (std::exception& e) {
				std::cout << e.what();
			}

			if (!is_ok)
				this->~FirBilder();
		}*/


		explicit FirBilder(FRsdkTypes::FaceVacsConfiguration configuration)
		{
			init(configuration);
		}

		FRsdkFir build(const std::string& bytes) override
		{
			if (bytes.size() <= 0)
				throw std::exception("Fir builder bytes size == 0");

			std::istringstream fir_bytestring_stream(bytes);
			auto fr(fir_builder_->build(fir_bytestring_stream));

			return FRsdkFir(new FRsdk::FIR(fr));
		}

	private:

		bool init(FRsdkTypes::FaceVacsConfiguration configuration)
		{
			try
			{				
				fir_builder_ = new FRsdk::FIRBuilder(*configuration);
				return true;
			}
			catch (std::exception& e) { std::cout << e.what(); return false; }
		}

	private:
		FRsdkFirBuilder            fir_builder_;
	};

	typedef std::shared_ptr<FirBilder> FirBuilderRef;
}

#endif
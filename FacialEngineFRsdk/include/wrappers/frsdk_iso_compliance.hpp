#ifndef FRsdkIsoCompliance_INCLUDED
#define FRsdkIsoCompliance_INCLUDED

#include <common/iface_characteristics.hpp>
#include <frsdk/portraittests.h>

#include "protobufs/bio_service.pb.h"

#include <ppl.h>

namespace FRsdkEntities
{	
	class FRsdkIsoComplianceResult
	{
	public:
		int analyze(const FRsdk::ISO_19794_5::FullFrontal::Compliance& iso_compliance)
		{
			auto result(0);	
			concurrency::parallel_invoke(
				[&](){setIsoComplianceMain(result, iso_compliance);	      },
				[&](){setIsoComplianceBest(result, iso_compliance);	      },
				[&](){setIsoComplianceAdditional(result, iso_compliance);	}		
		  );
		
			return result;
		}
			
		//TODO make in parallel
		bool isIsoCompliant(int flag)
		{
			return    hasFlag(flag, BioService::OnlyOneFaceVisible)
				     && hasFlag(flag, BioService::goodVerticalFacePosition)
				     && hasFlag(flag, BioService::horizontallyCenteredFace)
				     && hasFlag(flag, BioService::widthOfHead)
				     && hasFlag(flag, BioService::lengthOfHead)
				     && hasFlag(flag, BioService::resolution)
				     && hasFlag(flag, BioService::goodExposure)
				     && hasFlag(flag, BioService::goodGrayScaleProfile)
				     && hasFlag(flag, BioService::hasNaturalSkinColour)
				     && hasFlag(flag, BioService::noHotSpots)
				     && hasFlag(flag, BioService::isFrontal)
				     && hasFlag(flag, BioService::isLightingUniform)
				     && hasFlag(flag, BioService::noTintedGlasses)
				     && hasFlag(flag, BioService::isSharp);
		}

		//TODO make in parallel
		bool isIsoBestPractices(int flag)
		{
			return   hasFlag(flag, BioService::widthOfHeadBestPractice)
				    && hasFlag(flag, BioService::lengthOfHeadBestPractice)
				    && hasFlag(flag, BioService::resolutionBestPractice)
				    && hasFlag(flag, BioService::imageWidthToHeightBestPractice)
				    && hasFlag(flag, BioService::isBackgroundUniformBestPractice)
				    && hasFlag(flag, BioService::isFrontalBestPractice)
				    && hasFlag(flag, BioService::eyesGazeFrontalBestPractice)
				    && hasFlag(flag, BioService::eyesNotRedBestPractice)
				    && hasFlag(flag, BioService::eyesOpenBestPractice)
				    && hasFlag(flag, BioService::mouthClosedBestPractice);
		}

	private:
		void setFlag(int& value, bool flag, BioService::IsoComplianceTemplate place){
			if (flag)
				value |= 1 << place;
		}

		bool hasFlag(int flag, BioService::IsoComplianceTemplate place){
			return (flag & place) == place;
		}

		void setIsoComplianceMain(int& result, const FRsdk::ISO_19794_5::FullFrontal::Compliance& iso_compliance)
		{
			
			concurrency::parallel_invoke(
				[&](){setFlag(result, iso_compliance.onlyOneFaceVisible()      , BioService::OnlyOneFaceVisible);				},
				[&](){setFlag(result, iso_compliance.goodVerticalFacePosition(), BioService::goodVerticalFacePosition);	},
				[&](){setFlag(result, iso_compliance.horizontallyCenteredFace(), BioService::horizontallyCenteredFace);	},
				[&](){setFlag(result, iso_compliance.widthOfHead()             , BioService::widthOfHead);							},
				[&](){setFlag(result, iso_compliance.lengthOfHead()            , BioService::lengthOfHead);							},
			  [&](){setFlag(result, iso_compliance.resolution()              , BioService::resolution);								},
			  [&](){setFlag(result, iso_compliance.goodExposure()            , BioService::goodExposure);							},						     
			  [&](){setFlag(result, iso_compliance.goodGrayScaleProfile()    , BioService::goodGrayScaleProfile);			},									
			  [&](){setFlag(result, iso_compliance.hasNaturalSkinColour()    , BioService::hasNaturalSkinColour);			},									
			  [&](){setFlag(result, iso_compliance.noHotSpots()              , BioService::noHotSpots);								}
			);
			
		}

		void setIsoComplianceBest(int& result, const FRsdk::ISO_19794_5::FullFrontal::Compliance& iso_compliance)
		{
			/*
			
			concurrency::parallel_invoke(

			[&]() {setFlag(result, iso_compliance.widthOfHeadBestPractice(), BioService::widthOfHeadBestPractice);								 },
			[&]() {setFlag(result, iso_compliance.lengthOfHeadBestPractice(), BioService::lengthOfHeadBestPractice);							 },
			[&]() {setFlag(result, iso_compliance.resolutionBestPractice(), BioService::resolutionBestPractice);									 },
			[&]() {setFlag(result, iso_compliance.imageWidthToHeightBestPractice(), BioService::imageWidthToHeightBestPractice);	 },
																																																														
			[&]() {setFlag(result, iso_compliance.isBackgroundUniformBestPractice(), BioService::isBackgroundUniformBestPractice); },
			[&]() {setFlag(result, iso_compliance.eyesOpenBestPractice(), BioService::eyesOpenBestPractice);											 },
			[&]() {setFlag(result, iso_compliance.eyesGazeFrontalBestPractice(), BioService::eyesGazeFrontalBestPractice);				 },
			[&]() {setFlag(result, iso_compliance.eyesNotRedBestPractice(), BioService::eyesNotRedBestPractice);									 },
			[&]() {setFlag(result, iso_compliance.mouthClosedBestPractice(), BioService::mouthClosedBestPractice);  					     }
			);
			*/
		}

		void setIsoComplianceAdditional(int& result, const FRsdk::ISO_19794_5::FullFrontal::Compliance& iso_compliance)
		{			
			
			concurrency::parallel_invoke(
				[&]()  {setFlag(result, iso_compliance.isFrontal(), BioService::isFrontal);											},									
		   	[&]()  {setFlag(result, iso_compliance.isLightingUniform(), BioService::isLightingUniform);			},										
		  	[&]()  {setFlag(result, iso_compliance.noTintedGlasses(), BioService::noTintedGlasses);					},														
		  	[&]()  {setFlag(result, iso_compliance.isSharp(), BioService::isSharp);													}												

			);
			
		}
	};


	class FRsdkIsoCompliance : public BioContracts::IComlianceIsoTemplate
	{
	public:
		virtual ~FRsdkIsoCompliance() {}

		bool isoCompatible()  const override	{
			return iso_compatible_;
		}

		int  isoTemplate() const	override{
			return iso_template_;
		}

		bool bestPracticeCompatible() const override {
			return iso_compatible_best_practices_;
		}

		//private:
		explicit FRsdkIsoCompliance( const FRsdk::ISO_19794_5::FullFrontal::Compliance& iso_compliance)
		              	           : iso_template_(0), iso_compatible_(false), iso_compatible_best_practices_(false)
		{
			FRsdkIsoComplianceResult compliance;
			iso_template_                  = compliance.analyze(iso_compliance);
			iso_compatible_                = compliance.isIsoCompliant(iso_template_);
			iso_compatible_best_practices_ = compliance.isIsoBestPractices(iso_template_);
		}

	private:
		int  iso_template_;
		bool iso_compatible_;
		bool iso_compatible_best_practices_;

	};

	typedef std::shared_ptr<FRsdkIsoCompliance> FRsdkIsoCompliancePtr;

}
#endif
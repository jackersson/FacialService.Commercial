#ifndef FRsdkFaceCharacteristic_INCLUDED
#define FRsdkFaceCharacteristic_INCLUDED

#include "wrappers/frsdk_iso_compliance.hpp"
#include "wrappers/frsdk_basic_wrappers.hpp"

namespace FRsdkEntities
{
	class FRsdkFaceCharacteristic : public BioContracts::IFaceCharacteristics
	{
	public:
		explicit FRsdkFaceCharacteristic(const FRsdk::Portrait::Characteristics& portrait)
			: portrait_characteristics_(portrait)
			, face_center_(0, 0)
			, bounding_box_()
		{}

		virtual ~FRsdkFaceCharacteristic() {}

		float eyeDistance() const	override {
			return portrait_characteristics_.eyeDistance();
		}

		float glasses() const	override {
			return portrait_characteristics_.glasses();
		}

		float naturalSkinColor() const override	{
			return portrait_characteristics_.naturalSkinColour();
		}

		float headWidth() const override{
			return portrait_characteristics_.widthOfHead();
		}

		float headLength() const override{
			return portrait_characteristics_.lengthOfHead();
		}

		float poseAngleRoll() const override{
			return portrait_characteristics_.poseAngleRoll();
		}

		void set_eyes_characteristics(FRsdkEyesW& eyes) const 
		{
			set_eye_characteristics(eyes.eyes_.first);
			set_eye_characteristics(eyes.eyes_.second, false);
		}

		void set_confidence( float confidence) const	{
			
		}

		float confidence() const override {
			return 0.0f;
		}

	private:
		void set_eye_characteristics( FRsdkEye& eye, bool left = true )	const
		{
			auto is_open         = left ? portrait_characteristics_.eye0Open() 
				                          : portrait_characteristics_.eye1Open();
			auto is_red          = left ? portrait_characteristics_.eye0Red()
				                          : portrait_characteristics_.eye1Red();
			auto is_tinted       = left ? portrait_characteristics_.eye0Tinted() 
				                          : portrait_characteristics_.eye1Tinted();
			auto is_gaze_frontal = left ? portrait_characteristics_.eye0GazeFrontal() 
				                          : portrait_characteristics_.eye1GazeFrontal();			
			eye.set_additional_info(is_open, is_red, is_tinted, is_gaze_frontal);
		}
	public:
		float chin() const override{
			return portrait_characteristics_.chin();
		}

		float crown() const	override{
			return portrait_characteristics_.crown();
		}

		float leftEar() const	override{
			return portrait_characteristics_.ear0();
		}

		float rightEar() const override{
			return portrait_characteristics_.ear1();
		}


		BioContracts::Ethnicity ethnicity() const	override {

			auto eth = portrait_characteristics_.ethnicity();

			if (eth.asian > std::max(eth.black, eth.white))
				return BioContracts::Ethnicity::Asian;
			else if (eth.black > std::max(eth.asian, eth.white))
				return BioContracts::Ethnicity::Black;

			return BioContracts::Ethnicity::White;
		}

		float mouthClosed() const override	{
			return portrait_characteristics_.mouthClosed();
		}

		const BioContracts::IPosition& faceCenter() const override	{
			face_center_.copyFrom(portrait_characteristics_.faceCenter());
			return face_center_;
		}

		const BioContracts::IBox& faceBox() const	override{
			bounding_box_.copyFrom(FRsdk::Portrait::earToEarChinCrownSurroundingBox(portrait_characteristics_));
			return bounding_box_;
		}

		unsigned int faceSize() const	{
			return faceBox().size();
		}

		unsigned int age() const	override{
			return portrait_characteristics_.age();
		}

		bool isMale() const	override{
			return (portrait_characteristics_.isMale() > 0.5f);
		}

		const FRsdk::Portrait::Characteristics& portraitCharacteristics() const	{
			return portrait_characteristics_;
		}
		
	private:
		FRsdk::Portrait::Characteristics  portrait_characteristics_;

		mutable FRsdkPositionW face_center_;
		mutable FRsdkBoxW      bounding_box_;
		//static const float HALF_PROBABILITY         = 0.5f;
	};

	typedef std::shared_ptr<FRsdkFaceCharacteristic> FRsdkFaceCharacteristicPtr;
}
#endif
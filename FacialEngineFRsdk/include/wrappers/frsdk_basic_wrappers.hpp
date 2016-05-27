#ifndef FRsdkBasicWrappers_INCLUDED
#define FRsdkBasicWrappers_INCLUDED

#include "common/face_features.hpp"

#include <frsdk/position.h>
#include <frsdk/face.h>
#include <frsdk/eyes.h>

namespace FRsdkEntities
{
	class FRsdkPositionW : public BioContracts::IPosition
	{
	public:
		explicit FRsdkPositionW( const FRsdk::Position& position) 
			                     : position_(position.x(), position.y()) {}

		explicit FRsdkPositionW( float xx = 0, float yy = 0 ) 
			                     : position_(xx, yy) {}

		void copyFrom(const FRsdk::Position& position)
		{
			position_.first = position.x();
			position_.second = position.y();
		}

		void copyFrom(int xx, int yy)
		{
			position_.first  = static_cast<float>(xx);
			position_.second = static_cast<float>(yy);
		}

		void copyFrom(float xx, float yy)
		{
			position_.first = xx;
			position_.second = yy;
		}

		float x() const override {
			return position_.first;
		}

		float y() const override{
			return position_.second;
		}

	private:
		std::pair<float, float> position_;
	};


	class FRsdkFaceW : public BioContracts::IFace
	{
	public:
		explicit FRsdkFaceW(const FRsdk::Face::Location& location) : face_(location) {
			std::cout << "face : " << location.pos.x() << " " << location.pos.y() << std::endl;
		}

		float confidence() const override {
			return face_.confidence;
		}

		float width() const override{
			return face_.width;
		}

		float rotationAngle() const override {
			return face_.rotationAngle;
		}

		const FRsdk::Face::Location& instance() const {
			return face_;
		}

	private:
		FRsdk::Face::Location face_;
	};

	class FRsdkEye : public BioContracts::IEye
	{
	public:
		explicit FRsdkEye( const FRsdk::Position& pos, float confidence) 
		                 : position_(pos) , confidence_(confidence)
										 , open_(0.0f)    , red_(0.0f)
										 , tinted_(0.0f)  , gaze_frontal_(0.0f)
		{
			std::cout << "eyes : " << pos.x() << " " << pos.y() << std::endl;
		}

		explicit FRsdkEye( const FRsdk::Position& pos, float confidence
			               , float open, float red
										 , float tinted, float gaze_frontal  )
										 : position_(pos) , confidence_(confidence)
										 , open_(open)    , red_(red)
										 , tinted_(tinted), gaze_frontal_(gaze_frontal)
		{
			
		}

		void set_additional_info( float open = 0.0f, float red = 0.0f
			                      , float tinted = 0.0f, float gaze_frontal = 0.0f)
		{
			open_         = open        ;
			red_          = red         ;
			tinted_       = tinted      ;
			gaze_frontal_ = gaze_frontal;
		}

		void set_confidence( float confidence )	{
			confidence_ = confidence;
		}

		const FRsdkPositionW& position() const	override{
			return position_;
		}

		float confidence() const override {
			return confidence_;
		}

		float open() const	override{
			return open_;
		}

		float red() const	override{
			return red_;
		}

		float tinted() const	override{
			return tinted_;
		}

		float gaze_frontal() const	override{
			return gaze_frontal_;
		}

	private:
		FRsdkPositionW position_  ;
		float          confidence_;

		float open_;
		float red_ ;
		float tinted_;
		float gaze_frontal_;		
	};

	class FRsdkFaceCharacteristic;
	class FRsdkEyesW : public BioContracts::IEyes
	{
	public:
		friend FRsdkFaceCharacteristic;
		explicit FRsdkEyesW( const FRsdk::Eyes::Location& location)
			                  : frsdk_eyes_(location)
			                  , eyes_( FRsdkEye(location.first , location.firstConfidence)
			                         , FRsdkEye(location.second, location.secondConfidence))
		{}

		const FRsdkEye& left() const override {
			return eyes_.first;
		}

		const FRsdkEye& right() const override{
			return eyes_.second;
		}

		const FRsdk::Eyes::Location& instance() const {
			return frsdk_eyes_;
		}
	private:
		FRsdk::Eyes::Location frsdk_eyes_;
		std::pair<FRsdkEye, FRsdkEye> eyes_;
	};

	class FRsdkBoxW : public BioContracts::IBox
  {
  public:
  	explicit FRsdkBoxW( const FRsdk::Box& box)
			                : origin_(static_cast<float>(box.originx()), static_cast<float>(box.originy()))
											, end_(static_cast<float>(box.endx()), static_cast<float>(box.endy()))
  	{}

		explicit FRsdkBoxW( float originx = 0.0f, float originy = 0.0f
			                , float endx    = 0.0f, float endy = 0.0f)
		        	        : origin_(originx, originy)
							        , end_(endx, endy)
  	{}

		void copyFrom(const FRsdk::Box& box)
		{		
			origin_.copyFrom(box.originx(), box.originy());
			end_   .copyFrom(box.endx(), box.endy());		
		}
  
		const BioContracts::IPosition& origin() const override {
  		return origin_;
  	}
  
		const BioContracts::IPosition& end() const override {
  		return end_;
  	}
		
		int size() const override {
			return static_cast<int>( std::abs((origin_.x() - end_.x()) * (origin_.y() - end_.y())) );
		}  	  		
  	
  private:  	
  	FRsdkPositionW origin_;
		FRsdkPositionW end_;
  };
}
#endif
#ifndef FaceFeatures_INCLUDED
#define FaceFeatures_INCLUDED

#include <iostream>

namespace BioContracts
{
	class Position
	{
	public:	
		Position() {}
		Position(const int& x, const int& y) : p((float)x, (float)y) { }
		Position(const float& x, const float& y) : p(x, y) { }
		Position(const Position& rhs) : p(rhs.p) { }

		float x() const { return p.first; }
	
		float y() const { return p.second; }
	private:
		std::pair<float, float> p;
	};

	class SurroundingBox
	{
	public:
		SurroundingBox(int originx, int originy, int endx, int endy)
		{
			Position origin_temp(originx, originy);
			Position end_temp   (endx   , endy);
			box_ = std::pair<Position, Position>( origin_temp, end_temp );
		}
		SurroundingBox(const Position& origin, const Position& end) : box_(origin, end) {}
		SurroundingBox(const SurroundingBox& rhs) : box_(rhs.box_) { }

		Position origin() const { return box_.first; }

		Position end   () const { return box_.second; }
	private:
		std::pair<Position, Position> box_;
	};

	class FaceLocation
	{
	public :
		FaceLocation( const Position& p, float w
			          , float c = 0.0f, float rotationAngle_ = 0.0f)
			          : pos(p), width(w),
			            confidence(c), rotationAngle(rotationAngle_) { }

		Position pos;       
		float    width;        
		float    confidence;   
		float    rotationAngle;
	};

	class EyeLocation
	{
	public:
		EyeLocation( const Position& pos
			         , float conf            = 0.0f
			         , bool  is_open         = false
			         , bool  is_red          = false
			         , bool  is_tined        = false
			         , float gaze_frontal = 0.0f	)
							 : position(pos)    , confidence(conf)
			         , isOpen(is_open)  , isRed(is_red)
							 , isTined(is_tined), gazeFrontal(gaze_frontal)
		{ }

		Position position     ;         
		float    confidence   ;
		bool     isOpen       ;
	  bool     isRed        ;
		bool     isTined      ;
		float    gazeFrontal;
	};

	class EyesDetails
	{
	public:
		EyesDetails(const EyeLocation& origin, const EyeLocation& end) : eyes_(origin, end) {}
		EyesDetails(const EyesDetails& rhs) : eyes_(rhs.eyes_) { }

		EyeLocation left() const { return eyes_.first; }

		EyeLocation right() const { return eyes_.second; }
	private:
		std::pair<EyeLocation, EyeLocation> eyes_;
	};

	

	enum Gender
	{
		  None
		, Male
		, Female		
	};

	enum Ethnicity
	{
		  Undefined
		, White
		, Black
		, Asian
	};
}

#endif
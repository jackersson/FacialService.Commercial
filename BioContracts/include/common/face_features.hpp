#ifndef FaceFeatures_INCLUDED
#define FaceFeatures_INCLUDED

#include <iostream>

namespace BioContracts
{
	class Position
	{
	public:		
		Position(const float& x, const float& y) : p(x, y) { }
		Position(const Position& rhs) : p(rhs.p) { }

		float x() const { return p.first; }
	
		float y() const { return p.second; }
	private:
		std::pair<float, float> p;
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
		EyeLocation( const Position& f, const Position& s,
			           float fc = 0.0f, float sc = 0.0f)
			         : first(f), second(s),
			           firstConfidence(fc), secondConfidence(sc) { }

		Position first;         
		Position second;

		float firstConfidence;  
		float secondConfidence; 
	};
}

#endif
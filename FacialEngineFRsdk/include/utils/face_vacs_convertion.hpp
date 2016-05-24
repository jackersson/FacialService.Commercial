#ifndef FaceVacsCovertion_INCLUDED
#define FaceVacsCovertion_INCLUDED

#include "utils/face_vacs_includes.hpp"
#include "common/face_features.hpp"

#include <frsdk/portrait.h>

namespace BioFacialEngine
{
	/*
	class FaceVacsConvertion
	{
	public:
		BioContracts::FaceLocation toFaceLocation(const FRsdk::Face::Location& face) const
		{
			BioContracts::FaceLocation face_location(toPosition(face.pos)
				                                      , face.width
				                                      , face.confidence
				                                      , face.rotationAngle);
			return face_location;
		}

		BioContracts::EyesDetails toEyesDetails( const FRsdk::Eyes::Location& eyes
			                                     , const FRsdk::Portrait::Characteristics& portrait
																					 , float threshold																  ) const
		{
			BioContracts::EyeLocation left( toEyesLocation(eyes.first
				                            , eyes.firstConfidence
				                            , portrait.eye0GazeFrontal()
																		, portrait.eye0Open()  > threshold
																		, portrait.eye0Red()   > threshold
																		, portrait.eye0Tinted()> threshold));

			BioContracts::EyeLocation right( toEyesLocation(eyes.second
				                             , eyes.secondConfidence
				                             , portrait.eye1GazeFrontal()
																		 , portrait.eye1Open()  > threshold
																		 , portrait.eye1Red()   > threshold
																		 , portrait.eye1Tinted()> threshold));

		  BioContracts::EyesDetails details(left, right);
		  return details;
		}

		BioContracts::EyeLocation toEyesLocation( const FRsdk::Position& pos
			                                      , float confidence   = 0.0f
			                                      , float gaze_frontal = 0.0f
			                                      , bool  is_open      = false
			                                      , bool  is_red       = false
			                                      , bool  is_tined     = false ) const
		{
			
			BioContracts::EyeLocation eyes_location(  toPosition(pos), confidence																						
																							, is_open	, is_red, is_tined, gaze_frontal );
			return eyes_location;
		}
		

		BioContracts::Position toPosition(const FRsdk::Position& pos) const 
		{
			BioContracts::Position position(pos.x(), pos.y());
			return position;
		}

		BioContracts::SurroundingBox toSurroundingBox(const FRsdk::Box& box) const
		{			
			BioContracts::SurroundingBox sbox(box.originx(), box.originy(), box.endx(), box.endy());
			return sbox;
		}
	};
	*/
}

#endif
#ifndef IFaceCharacteristics_INCLUDED
#define IFaceCharacteristics_INCLUDED

#include "face_features.hpp"
#include <string>

namespace BioContracts
{
	class IFaceCharacteristics
	{
	public:
		virtual unsigned int age() const = 0;

		virtual bool  hasFace() const = 0;
		//virtual const FaceLocation faceLocation() const = 0;

		virtual bool  hasEyes() const = 0;
		virtual const EyesDetails eyesDetails() const = 0;

		virtual bool  isMale()                      const = 0;
		virtual bool  isCompliant()                 const = 0;
		virtual int   isoCompliantResult()          const = 0;
		virtual bool  isCompliantWithBestPractice() const = 0;
		virtual float eyeDistance()                 const = 0;
		virtual float glasses()                     const = 0;
		virtual float naturalSkinColor()            const = 0;
		virtual float headWidth()                   const = 0;
		virtual float headLength()                  const = 0;
		virtual float poseAngleRoll()               const = 0;
		virtual float chin()                        const = 0;
		virtual float crown()                       const = 0;
		virtual float leftEar()                     const = 0;
		virtual float rightEar()                    const = 0;
		virtual float confidence()                  const = 0;		
		virtual float mouthClosed()                 const = 0;	

		virtual const BioContracts::Position         faceCenter()   const = 0;
		virtual const BioContracts::SurroundingBox   faceBox()      const = 0;
		virtual const std::string&                   firTemplate()  const = 0;
		virtual const BioContracts::Ethnicity        ethnicity()    const = 0;

	};
}

#endif
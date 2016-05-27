#ifndef IFaceCharacteristics_INCLUDED
#define IFaceCharacteristics_INCLUDED

#include "face_features.hpp"

namespace BioContracts
{
	class IFaceCharacteristics
	{
	public:
		virtual ~IFaceCharacteristics() {}

		virtual unsigned int age() const = 0;	

		virtual bool  isMale()            const = 0;
		virtual float eyeDistance()       const = 0;
		virtual float glasses()           const = 0;
		virtual float naturalSkinColor()  const = 0;
		virtual float headWidth()         const = 0;
		virtual float headLength()        const = 0;
		virtual float poseAngleRoll()     const = 0;
		virtual float chin()              const = 0;
		virtual float crown()             const = 0;
		virtual float leftEar()           const = 0;
		virtual float rightEar()          const = 0;			
		virtual float mouthClosed()       const = 0;	
		virtual float confidence()        const = 0;

		virtual const IPosition& faceCenter()   const = 0;
		virtual const IBox&      faceBox()      const = 0;
		virtual Ethnicity        ethnicity()    const = 0;

	};

	class IComlianceIsoTemplate
	{
	public:
		virtual ~IComlianceIsoTemplate() {}

		virtual bool isoCompatible()          const = 0;																          
		virtual int  isoTemplate()            const = 0;
		virtual bool bestPracticeCompatible() const = 0;		
	};



}

#endif
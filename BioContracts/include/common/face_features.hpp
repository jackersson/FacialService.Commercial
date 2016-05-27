#ifndef FaceFeatures_INCLUDED
#define FaceFeatures_INCLUDED

#include <memory>

namespace BioContracts
{
	class IPosition
	{
	public:
		virtual ~IPosition() {}

		virtual float x() const = 0;
		virtual float y() const = 0;
	};

	class Position : public IPosition
	{
	public:
		Position(float x, float y) :par_(x, y) {}

		float x() const override{
			return par_.first;
		}

		float y() const override {
			return par_.second;
		}

	private:
		std::pair<float, float> par_;
	};

	class IFace
	{
	public:
		virtual ~IFace() {}

		virtual float confidence()    const = 0;
		virtual float width()         const = 0;
		virtual float rotationAngle() const = 0;		
	};

	class IEye
	{
	public:
		virtual ~IEye() {}

		virtual const IPosition& position() const = 0;

		virtual float confidence()   const = 0;															   
		virtual float open()         const = 0;
		virtual float red()          const = 0;
		virtual float tinted()       const = 0;
		virtual float gaze_frontal() const = 0;
	};

	class IEyes
	{
	public:
		virtual ~IEyes() {}

		virtual const IEye& left()  const = 0;
		virtual const IEye& right() const = 0;
	};

	class IBox
	{
	public:
		virtual ~IBox() {}

		virtual const IPosition& origin() const = 0;
		virtual const IPosition& end   () const = 0;	

		virtual int size() const = 0;
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
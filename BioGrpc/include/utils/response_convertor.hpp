#ifndef ResponseConvertor_INCLUDED
#define ResponseConvertor_INCLUDED

#include "protobufs/bio_service.pb.h"
#include "common/face_features.hpp"
//#include "frsdk_face_characteristics.hpp"

//#include <frsdk\face.h>

namespace BioGrpc
{
	class ResponseConvertor 
	{
	public:

		//const std::string& P

		BioService::PortraitCharacteristic*
			getPortraitCharacteristics(BioContracts::ImageCharacteristicsConstRef characteristics)
		{
			BioService::PortraitCharacteristic* proto	= new BioService::PortraitCharacteristic();
			/*
			for (size_t i = 0; i < characteristics->size(); ++i)
			{
				const BioContracts::IFaceCharacteristics& face = (*characteristics)[i];
				BioService::FaceCharacteristic* proto_face = proto->add_faces();
				updateFaceCharacteristicResponse(*proto_face, face);
			}
			/*
			for (auto it = characteristics.cbegin(); it != characteristics.cend(); ++it)
			{
				BioFacialEngine::FRsdkFaceCharacteristic face = *(it->get());
				BioService::FaceCharacteristic* proto_face = proto->add_faces();
				updateFaceCharacteristicResponse(*proto_face, face);
			}
			*/

			return proto;
		}


		
		BioService::FacialImage* getFacialImage( long owner_biometric_data_id
			                                     , const BioService::Photo& photo 
																					 , BioContracts::ImageCharacteristicsConstRef)
		{
			BioService::FacialImage* facial_image = new BioService::FacialImage();
			/*
			facial_image->set_allocated_image(new BioService::Photo(photo));

			for (auto it = characteristics.cbegin(); it != characteristics.cend(); ++it)
			{
				BioFacialEngine::FRsdkFaceCharacteristic face = *(it->get());
				BioService::FaceCharacteristic* proto_face = facial_image->add_faces();
				updateFaceCharacteristicResponse(*proto_face, face);
			}
			*/
		//	BioFacialEngine::FRsdkFaceCharacteristic face = characteristics.getEnrollmentAbleFace();

			//facial_image->set_template_(face.firTemplate());

			return facial_image;
		}

		//todo will send to database
		void updateFaceCharacteristicResponse( BioService::FaceCharacteristic& proto_face
			                                   , const BioContracts::IFaceCharacteristics& face)
		{			
			/*
			if (!face.hasFace())
				return;

		  FRsdk::Face::Location face_location = face.faceLocation();
		  proto_face.set_allocated_location(getBiometricLocation(face_location));
		  
		  proto_face.set_width(face_location.width);
		  
		  if (face.hasEyes())								
		  	proto_face.set_allocated_eyes(getEyesCharacteristics(face.eyesLocations()));
		  
		  proto_face.set_age(face.age());
		  
		  BioService::Gender gender = face.isMale() ? BioService::Gender::Male
		  	                                        : BioService::Gender::Female;
		  proto_face.set_gender(gender);			
			*/
		}
		
		BioService::EyesCharacteristic* getEyesCharacteristics(const BioContracts::EyeLocation& eyes)
		{
			BioService::EyesCharacteristic* proto_eyes = new BioService::EyesCharacteristic();

			proto_eyes->set_allocated_left_eye(
				getBiometricLocation(eyes.first, eyes.firstConfidence));

			proto_eyes->set_allocated_left_eye(
				getBiometricLocation(eyes.second, eyes.secondConfidence));
		}

		BioService::BiometricLocation* getBiometricLocation(const BioContracts::FaceLocation& face)
		{
			return getBiometricLocation(face.pos, face.confidence);
		}

		BioService::BiometricLocation* getBiometricLocation( const BioContracts::Position& pos
			                                                 , float confidence)
		{
			return getBiometricLocation(pos.x(), pos.y(), confidence);
		}


		BioService::BiometricLocation* getBiometricLocation( float xpos
			                                                 , float ypos
			                                                 , float confidence)
		{
			BioService::BiometricLocation* proto_location = new BioService::BiometricLocation();
			proto_location->set_xpos(xpos);
			proto_location->set_ypos(ypos);
			proto_location->set_confidence(confidence);

			return proto_location;
		}
	};
}

#endif
#ifndef ResponseConvertor_INCLUDED
#define ResponseConvertor_INCLUDED

#include "protobufs\bio_service.pb.h"
#include "common/face_features.hpp"
#include "common\iface_characteristics.hpp"
#include "common\iimage_characteristic.hpp"
#include <common/matches.hpp>

namespace BioGrpc
{
	class ResponseConvertor 
	{
	public:
		/*
		BioService::VerificationResult* getProtoVerificationResult( BioContracts::VerificationResult& vr)
		{
			BioService::VerificationResult* verification_data = new BioService::VerificationResult();

			BioContracts::Matches matches = vr.matches();
			for (auto it = matches.cbegin(); it != matches.cend(); ++it)
			{
				BioService::MatchResult* match = verification_data->add_matches();
				getMatchResult(*match, *it);
			}

			BioService::PortraitCharacteristic* proto_first = verification_data->add_portraits();			
			updatePortraitCharacteristics(*proto_first, vr.first());
		
			BioService::PortraitCharacteristic* proto_second = verification_data->add_portraits();
			updatePortraitCharacteristics(*proto_second, vr.second());

			return verification_data;
		}

		void getMatchResult(BioService::MatchResult& proto_match, const BioContracts::Match& match)
		{
			proto_match.set_comparison_face_id(match.comparisonFaceId());
			proto_match.set_target_face_id    (match.targetFaceId()    );
			proto_match.set_match             (match.match()           );
		}
		*/

		BioService::PortraitCharacteristic*
			getPortraitCharacteristics(BioContracts::IImageInfoPtr characteristics)
		{
			BioService::PortraitCharacteristic* proto	= new BioService::PortraitCharacteristic();	

			updatePortraitCharacteristics(*proto, characteristics);

			return proto;
		}

		void updatePortraitCharacteristics( BioService::PortraitCharacteristic& proto
			                                , BioContracts::IImageInfoPtr characteristics)
		{
			for (size_t i = 0; i < characteristics->size(); ++i)
			{
				const BioContracts::IFaceInfo& face = (*characteristics)[i];
				BioService::FaceCharacteristic* proto_face = proto.add_faces();
				updateFaceCharacteristicResponse(*proto_face, face.characteristics());
				update_iso_compliance_info(*proto_face, face.iso_compliance());
				update_min_face_info      (*proto_face, face.face());
				update_eyes_info          (*proto_face, face.eyes());
			}
		}
		
		BioService::FacialImage* getFacialImage( long owner_biometric_data_id
			                                     , const BioService::Photo& photo 
																					 , BioContracts::IImageInfoPtr )
		{
			auto facial_image = new BioService::FacialImage();
			
			facial_image->set_allocated_image(new BioService::Photo(photo));

			//for (auto it = characteristics.cbegin(); it != characteristics.cend(); ++it)
			//{
			//	BioFacialEngine::FRsdkFaceCharacteristic face = *(it->get());
			//	BioService::FaceCharacteristic* proto_face = facial_image->add_faces();
			//	updateFaceCharacteristicResponse(*proto_face, face);
			//}
			//*/
		//	BioFacialEngine::FRsdkFaceCharacteristic face = characteristics.getEnrollmentAbleFace();

			//facial_image->set_template_(face.firTemplate());

			return facial_image;
		}

		//todo will send to database
		void updateFaceCharacteristicResponse( BioService::FaceCharacteristic& proto_face
			                                   , const BioContracts::IFaceCharacteristics& face)
		{	
			proto_face.set_allocated_box(toProtoSurroundingBox(face.faceBox()));
			proto_face.set_eye_distance (face.eyeDistance());
			proto_face.set_allocated_face_center(toProtoPosition(face.faceCenter()));
			proto_face.set_glasses(face.glasses());
			proto_face.set_natural_skin_color(face.naturalSkinColor());
			proto_face.set_head_width(face.headWidth());
			proto_face.set_head_length(face.headLength());
			proto_face.set_pose_angle_roll(face.poseAngleRoll());
			proto_face.set_chin(face.chin());
			proto_face.set_crown(face.crown());
			proto_face.set_left_ear(face.leftEar());
			proto_face.set_rigth_ear(face.rightEar());
			proto_face.set_confidence(face.confidence());
			proto_face.set_mouth_closed(face.mouthClosed());
			proto_face.set_ethnithity(toProtoEthnithity(face.ethnicity()));
			proto_face.set_age(face.age());			
			proto_face.set_gender(toProtoGender(face.isMale()));
				
		}

		void update_iso_compliance_info( BioService::FaceCharacteristic& proto_face
		                         , const BioContracts::IComlianceIsoTemplate& face) const
		{
			proto_face.set_compliance_iso(face.isoTemplate()            );
			proto_face.set_good          (face.isoCompatible()          );
			proto_face.set_best_practices(face.bestPracticeCompatible() );
		}

		void update_min_face_info( BioService::FaceCharacteristic& proto_face
		                         , const BioContracts::IFace& face) const
		{
			proto_face.set_confidence(face.confidence());			
		}

		void update_eyes_info( BioService::FaceCharacteristic& proto_face
			                   , const BioContracts::IEyes& eyes)
		{
			proto_face.set_allocated_eyes(toProtoEyesCharacteristics(eyes));
		}

		BioService::Gender toProtoGender(bool is_male)
		{
			return is_male ? BioService::Gender::Male
				             : BioService::Gender::Female;
		}

		BioService::SurroundingBox* toProtoSurroundingBox(const BioContracts::IBox& box)
		{
			auto proto_box = new BioService::SurroundingBox();
			proto_box->set_allocated_begin(toProtoPosition(box.origin()));
			proto_box->set_allocated_end  (toProtoPosition(box.end()));
			return proto_box;
		}

		BioService::Ethnicity toProtoEthnithity( BioContracts::Ethnicity eth )
		{
			switch(eth)
			{
			  case BioContracts::Ethnicity::Asian: return BioService::Ethnicity::Asian;
			  case BioContracts::Ethnicity::White: return BioService::Ethnicity::White;
			  case BioContracts::Ethnicity::Black: return BioService::Ethnicity::Black;

				default:
					return BioService::Ethnicity::Unpredictable;
			}
		}
		
		BioService::EyesCharacteristic* toProtoEyesCharacteristics(const BioContracts::IEyes& eyes)
		{
			auto proto_eyes = new BioService::EyesCharacteristic();

			proto_eyes->set_allocated_left_eye(
				toProtoDetailedEyes(eyes.left()));

			proto_eyes->set_allocated_left_eye(
				toProtoDetailedEyes(eyes.right()));

			return proto_eyes;
		}

		BioService::DetailedEyeCharacteristic* toProtoDetailedEyes(const BioContracts::IEye& eye)
		{
			auto proto_detailed_eyes = new BioService::DetailedEyeCharacteristic();
			proto_detailed_eyes->set_allocated_position(toProtoPosition(eye.position()));
			proto_detailed_eyes->set_confidence        (eye.confidence()    );
			proto_detailed_eyes->set_is_gaze_frontal   (eye.gaze_frontal()  );
			proto_detailed_eyes->set_is_open           (eye.open()          );
			proto_detailed_eyes->set_is_red            (eye.red()           );
			proto_detailed_eyes->set_is_tined          (eye.tinted()        );
			return proto_detailed_eyes;
		}

		//BioService::Position* toProtoPosition(const BioContracts::FaceLocation& face)
		//{
		//	return toProtoPosition(face.pos);
	//	}

		BioService::Position* toProtoPosition( const BioContracts::IPosition& pos  )
		{
			return toProtoPosition(pos.x(), pos.y());
		}
		
		BioService::Position* toProtoPosition( float xpos, float ypos	)
		{
			auto proto_position = new BioService::Position();
			proto_position->set_x(xpos);
			proto_position->set_y(ypos);
			return proto_position;
		}
	};
}

#endif
#ifndef ResponseConvertor_INCLUDED
#define ResponseConvertor_INCLUDED

#include "protobufs\bio_service.pb.h"
#include "common/face_features.hpp"
#include "common\iface_characteristics.hpp"
#include "common\iimage_characteristic.hpp"
#include <common/matches.hpp>
#include "common/verification_result.hpp"
#include "common/identification_result.hpp"

#include <ppl.h>

namespace BioGrpc
{
	class ResponseConvertor 
	{
	public:
	  //test
		BioService::FaceSearchResponse* get_face_search_result( BioContracts::IdentificationResultPtr identification_result)
		{
			BioService::FaceSearchResponse* result = new BioService::FaceSearchResponse();
	
			Concurrency::parallel_invoke(
			[&]()	{

				//Concurrency::parallel_for_each(identification_result->cbegin(), identification_result->cend(),
					//[&](BioContracts::IImageInfoPtr image)
				for (auto it = identification_result->cbegin(); it != identification_result->cend(); ++it)
				{
					BioService::PortraitCharacteristic* pch = result->add_portraits();
					updatePortraitCharacteristics(*pch, *it);
				}//);			  
			 },		
				 [&](){	update_face_search_result(*result, identification_result->matches());	});

			return result;
		}

		BioService::FaceSearchResponse* get_face_search_result( BioContracts::VerificationResultPtr verification_result)
		{
			BioService::FaceSearchResponse* result = new BioService::FaceSearchResponse();
			BioService::PortraitCharacteristic* target = result->add_portraits();
			BioService::PortraitCharacteristic* compare = result->add_portraits();
			Concurrency::parallel_invoke(
			[&]()	{
				updatePortraitCharacteristics(*target, verification_result->first());
			 },
			[&]() {				
				updatePortraitCharacteristics(*compare, verification_result->second());
		  },
				[&](){	update_face_search_result(*result, verification_result->matches());	});

			return result;
		}

		void update_face_search_result(BioService::FaceSearchResponse& proto, const BioContracts::Matches& mchs)
		{
			for (auto it = mchs.cbegin(); it != mchs.cend(); ++it)
			{
				auto mt = *it;				
				auto matches(mt.second);
				if (matches.size() <= 0)
					continue; 
					
				auto face_id(mt.first);
				update_mathces(*proto.add_matches(), face_id, matches);
			}
		}

		void update_mathces(BioService::Matches& proto_match, long face_id, const std::list<BioContracts::Match>& matches)
		{
			proto_match.set_face_id(face_id);

			for (auto m_iter = matches.begin(); m_iter != matches.end(); ++m_iter)						
				update_match(*proto_match.add_matches(), *m_iter);			
		}

		void update_match(BioService::Match& proto, const BioContracts::Match& match)
		{
			proto.set_face_id(match.face_id());
			proto.set_match  (match.match());
		}

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
			std::cout << "faces" << characteristics->size() << std::endl;
			for (size_t i = 0; i < characteristics->size(); ++i)
			{
				const BioContracts::IFaceInfo& face = (*characteristics)[i];
				BioService::FaceCharacteristic* proto_face = proto.add_faces();
				//auto start = clock();
				proto_face->set_id(face.id());
				proto_face->set_photoid(characteristics->id());
				Concurrency::parallel_invoke(
				  [&](){
					if (face.has_face_characteristics())
					  update_main_face_characteristic      (*proto_face, face.characteristics());	
				},
				  [&](){
					if (face.has_face_characteristics())
					  update_additional_face_characteristic(*proto_face, face.characteristics());	
				},
				  [&](){
					 if (face.has_iso_compliance())
				  	 update_iso_compliance_info(*proto_face, face.iso_compliance());			
				},
				  [&](){update_min_face_info      (*proto_face, face.face());												},
				  [&](){update_eyes_info          (*proto_face, face.eyes());												}
				);			
				
			}
		}
		/*
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

			//return facial_image;
		//}

		//todo will send to database
		void update_main_face_characteristic( BioService::FaceCharacteristic& proto_face
			                                   , const BioContracts::IFaceCharacteristics& face)
		{	
			//if (face == NULL)
			//	return;

			Concurrency::parallel_invoke(
			  [&](){proto_face.set_allocated_box(toProtoSurroundingBox(face.faceBox()));				},
				[&](){				}
				/*
			  [&](){proto_face.set_eye_distance (face.eyeDistance());													},
			  [&](){proto_face.set_allocated_face_center(toProtoPosition(face.faceCenter()));	},
			  [&](){proto_face.set_glasses(face.glasses());																		},
			  [&](){proto_face.set_natural_skin_color(face.naturalSkinColor());								},
			  [&](){proto_face.set_head_width(face.headWidth());																},
			  [&](){proto_face.set_head_length(face.headLength());															},
			  [&](){proto_face.set_pose_angle_roll(face.poseAngleRoll());											},
			  [&](){proto_face.set_chin(face.chin());																					},
			  [&](){proto_face.set_crown(face.crown());																				} */
			 
			);
				
		}

		void update_additional_face_characteristic(BioService::FaceCharacteristic& proto_face
			, const BioContracts::IFaceCharacteristics& face)
		{
			/*
			Concurrency::parallel_invoke(	
				[&](){proto_face.set_left_ear(face.leftEar());																		},
				[&](){proto_face.set_rigth_ear(face.rightEar());																	},		
				[&](){proto_face.set_mouth_closed(face.mouthClosed());														},
				[&](){proto_face.set_ethnithity(toProtoEthnithity(face.ethnicity()));						},
				[&](){proto_face.set_age(face.age());																						},
				[&](){proto_face.set_gender(toProtoGender(face.isMale()));												}
			);
			*/
		}

		void update_iso_compliance_info( BioService::FaceCharacteristic& proto_face
		                         , const BioContracts::IComlianceIsoTemplate& face) const
		{
			/*/
			Concurrency::parallel_invoke(
			[&](){proto_face.set_compliance_iso(face.isoTemplate()            );	 },
			[&](){proto_face.set_good          (face.isoCompatible()          );	 },
			[&](){proto_face.set_best_practices(face.bestPracticeCompatible() );	 }
			);
			*/
		}

		void update_min_face_info( BioService::FaceCharacteristic& proto_face
		                         , const BioContracts::IFace& face) const
		{
			proto_face.set_confidence(face.confidence());			
		}

		void update_eyes_info( BioService::FaceCharacteristic& proto_face
			                   , const BioContracts::IEyes& eyes)
		{
			//proto_face.set_allocated_eyes(toProtoEyesCharacteristics(eyes));
		}

		BioService::Gender toProtoGender(bool is_male)
		{
			return is_male ? BioService::Gender::Male
				             : BioService::Gender::Female;
		}

		BioService::Box* toProtoSurroundingBox(const BioContracts::IBox& box)
		{

			auto proto_box = new BioService::Box();
			const BioContracts::IPosition& origin = box.origin();
			const BioContracts::IPosition& end    = box.end   ();
			Concurrency::parallel_invoke(
			[&](){proto_box->set_allocated_begin(toProtoPosition(origin));},
			[&](){proto_box->set_width  ( std::abs(origin.x() - end.x()));	 },
			[&](){proto_box->set_height(std::abs(origin.y() - end.y()));	 }
			);
			return proto_box;
		}
		/*
		BioService::Eth toProtoEthnithity( BioContracts::Ethnicity eth )
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

			proto_eyes->set_allocated_right_eye(
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
		*/
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
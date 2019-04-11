// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"
#include "Misc/Paths.h"				// FPaths::ProjectContentDir() 사용을 위해...
#include "EngineUtils.h"			// Debug 출력을 위해...
#include "PBSAppVar.h"
#include "PBS/MotionDBforPBS.h"
#include "MyPawnVR.h"
#include "Engine.h"
#include "CoreMinimal.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "DrawDebugHelpers.h"
#include <sstream>

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	user_to_data_scale_t_.identity();

	flag_smooth = true;
	flag_first_aftr_reset = true;
	flag_hmd_retarget = true;
	flag_pelvis_retarget = true;
	flag_foot_retarget = true;
	flag_display_pose_data = true;
}

AMyActor::~AMyActor()
{
	PBSAppVar::DestroySingleton();
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();


	

	flag_first_aftr_reset = true;



	//////////////////////////////////////////////////////////////////////////////////////////
	//// 1. UE4 캐릭터 모델 준비
	//// 1.1 InitPoseableCharacter() 함수를 호출 
	// BluePrinter를 통해 지정된 캐릭터 모델을 c++ code에서 참조할 수 있도록 준비한다.
	// 구체적으로 말하면 멤버 변수인 'poseable_char_'와 'ml_u_posers_'를 사용 가능한 상태로 준비한다.
	// 함수가 성공적으로 처리된 경우 true가 리턴된다.
	// 반드시 InitPoseableCharacter() 함수의 내용도 살펴보자. 
	flag_valid_poseable_char_ = InitPoseableCharacter();




	//////////////////////////////////////////////////////////////////////////////////////////
	//// 2. 모션 캡쳐 데이터 로드
	//// 2.1 모션 데이터 파일의 위치(폴더)와 이름 지정
	// FPaths::ProjectContentDir() 는 UE4 프로젝트 폴터의 Content 폴더 위치를 리턴한다.
	FString T_pose_filename = FPaths::ProjectContentDir() + "bvh/bsh_T.bvh";	// T-Pose 자세를 저장하고있는 모션 데이터 파일
	FString motion_filename = FPaths::ProjectContentDir() + "bvh/bsh_dance.bvh"; // 로드해야할 모션 데이터 파일


	//// 2.2 'motion_filename' 로드
	// 모션 데이터 파일을 로드하고 그 내용을 'motion_' 객체에 담는다.
	// 반드시 T-Pose 자세를 포함하고 있는 데이터 파일을 첫번째 파라메터로 넣어야한다.
	// ML libaray에서는 T-Pose를 기준으로 상대적인 인체 자세의 변화를 분석하고 향후 모든 계산에 사용한다.
	ml::LoadBVH_UE4(T_pose_filename, motion_filename, motion_);

	const PBS::MotionDBforPBS *m_db = PBSAppVar::getSingleton()->motino_db();
	//m_db->getMotion(0)->Clone(&motion_);

	//// 2.3 모션 데이터 각 인체 부위 이름 지정
	// 각각의 모션 캡쳐 데이터는 파일마다 인체 부위에 대한 이름이 서로 다르게 기록되어있을 것이다.
	// ML library를 사용하기 위해서는 각 인체 부위에 사전에 약속된 이름(JointTag)을 부여하여야 한다.
	// SetJointTag("모션 캡쳐 데이터 상의 이름", "ML library에 정의 된 이름").
	motion_.editable_body()->SetJointTag("Hips", ml::PELVIS);
	motion_.editable_body()->SetJointTag("Spine", ml::SPINE);
	motion_.editable_body()->SetJointTag("Spine1", ml::SPINE1);
	motion_.editable_body()->SetJointTag("Spine2", ml::CHEST);
	motion_.editable_body()->SetJointTag("Neck", ml::NECK);
	motion_.editable_body()->SetJointTag("Head", ml::HEAD);

	motion_.editable_body()->SetJointTag("RightUpLeg", ml::R_HIP);
	motion_.editable_body()->SetJointTag("RightLeg", ml::R_KNEE);
	motion_.editable_body()->SetJointTag("RightFoot", ml::R_ANKLE);
	motion_.editable_body()->SetJointTag("RightToeBase", ml::R_FOOT);

	motion_.editable_body()->SetJointTag("LeftUpLeg", ml::L_HIP);
	motion_.editable_body()->SetJointTag("LeftLeg", ml::L_KNEE);
	motion_.editable_body()->SetJointTag("LeftFoot", ml::L_ANKLE);
	motion_.editable_body()->SetJointTag("LeftToeBase", ml::L_FOOT);

	//motion_.editable_body()->SetJointTag("RightShoulder", ml::R_CLAVICLE);
	motion_.editable_body()->SetJointTag("RightArm", ml::R_SHOULDER);
	motion_.editable_body()->SetJointTag("RightForeArm", ml::R_ELBOW);
	motion_.editable_body()->SetJointTag("RightHand", ml::R_WRIST);

	//motion_.editable_body()->SetJointTag("LeftShoulder", ml::L_CLAVICLE);
	motion_.editable_body()->SetJointTag("LeftArm", ml::L_SHOULDER);
	motion_.editable_body()->SetJointTag("LeftForeArm", ml::L_ELBOW);
	motion_.editable_body()->SetJointTag("LeftHand", ml::L_WRIST);



}
 

bool AMyActor::InitPoseableCharacter()
{
	//////////////////////////////////////////////////////////////////////////////////////////
	//// 1. 'james_posable_' 설정
	//// 1.1 현재 Actor가 소유하고 있는 모든 UPoseableMeshComponent 객체의 포인터를 받아서,
	////     'components' 배열에 담는다. 
	TArray<UPoseableMeshComponent*> components;
	GetComponents(components);

	//// 1.2 components 배열을 검사하여 이름이 "JamePoseable" 인 것을 찾고,
	////     그 포인터를 맴버 변수 'jame_poseable_' 에 저정한다.
	james_poseable_ = nullptr;
	for (int i = 0; i<components.Num(); i++)
	{
		if (components[i]->GetName() == "JamesPoseable")
		{
			james_poseable_ = (UPoseableMeshComponent*)components[i];
			// UE_LOG(LogTemp, Warning, TEXT("james_poseable_ = %s"), *(james_poseable_->GetName())); // 디버그 용 출력
		}
	}



	// "JamesPoseable" 찾기에 실패한 경우 return false.
	if (james_poseable_ == nullptr) return false;	

	// "JamesPoseable" 이 적절한 메시 모델을 소유하고 있지 안은 경우 return false.
	if (james_poseable_->SkeletalMesh == nullptr) return false;	




	//////////////////////////////////////////////////////////////////////////////////////////
	//// 2. 'ml_u_poser_' 설정
	//// 2.1 'james_poseable_' 포인터 값을 입력한다.
	ml_u_poser_.SetUPoseableMeshComponent(james_poseable_);

	//// 2.2 캐릭터 모델 인체 부위 이름 지정
	// 각각의 캐릭터 모델의 신체 부위 이름은 제작자가 임의로 부여하기 때문에 서로 다를 것이다.
	// ML library를 사용하기 위해서는 각 인체 부위에 사전에 약속된 이름(JointTag)을 부여하여야 한다.
	// SetJointTag("캐릭터 모델 상의 신체 이름", "ML library에 정의 된 이름").
	ml_u_poser_.SetJointTag("Bip001-Pelvis", ml::PELVIS); // pelvis
	ml_u_poser_.SetJointTag("Bip001-Spine", ml::SPINE); // spine_01
	ml_u_poser_.SetJointTag("Bip001-Spine1", ml::SPINE1); // spine_02
	ml_u_poser_.SetJointTag("Bip001-Spine2", ml::CHEST); // spine_03
	ml_u_poser_.SetJointTag("Bip001-Neck", ml::NECK); // neck_01
	ml_u_poser_.SetJointTag("Bip001-Head", ml::HEAD); // head

	ml_u_poser_.SetJointTag("Bip001-R-Thigh", ml::R_HIP); // thigh_r
	ml_u_poser_.SetJointTag("Bip001-R-Calf", ml::R_KNEE); // calf_r
	ml_u_poser_.SetJointTag("Bip001-R-Foot", ml::R_ANKLE); //foot_r
	ml_u_poser_.SetJointTag("Bip001-R-Toe0", ml::R_FOOT);//ball_r

	ml_u_poser_.SetJointTag("Bip001-L-Thigh", ml::L_HIP); // thigh_l
	ml_u_poser_.SetJointTag("Bip001-L-Calf", ml::L_KNEE); // calf_l
	ml_u_poser_.SetJointTag("Bip001-L-Foot", ml::L_ANKLE); //foot_l
	ml_u_poser_.SetJointTag("Bip001-L-Toe0", ml::L_FOOT);//ball_l
	

	ml_u_poser_.SetJointTag("Bip001-R-Clavicle", ml::R_CLAVICLE); // clavicle_r
	ml_u_poser_.SetJointTag("Bip001-R-UpperArm", ml::R_SHOULDER); //upperarm_r
	ml_u_poser_.SetJointTag("Bip001-R-Forearm", ml::R_ELBOW); // lowerarm_r
	ml_u_poser_.SetJointTag("Bip001-R-Hand", ml::R_WRIST); // hand_r

	ml_u_poser_.SetJointTag("Bip001-L-Clavicle", ml::L_CLAVICLE); // clavicle_l
	ml_u_poser_.SetJointTag("Bip001-L-UpperArm", ml::L_SHOULDER); //upperarm_l
	ml_u_poser_.SetJointTag("Bip001-L-Forearm", ml::L_ELBOW); // lowerarm_l
	ml_u_poser_.SetJointTag("Bip001-L-Hand", ml::L_WRIST); // hand_l

	//// 2.3 위에 설정된 내용을 바탕으로 'ml_u_poser' 내부 데이터를 업데이트한다.
	// 'BuildSkeleton()' 함수는 반드시 2.1과 2.2의 설정이 끝난후 실행되어야한다.
	ml_u_poser_.BuildSkeleton();
	

	calibed_T_[MS_L_ANKLE] = cml::MatrixTranslation(p0_[0], p0_[1], p0_[2]);
	calibed_T_[MS_R_ANKLE] = cml::MatrixTranslation(p1_[0], p1_[1], p1_[2]);
	calibed_T_[MS_L_WRIST] = cml::MatrixTranslation(p2_[0], p2_[1], p2_[2]);
	calibed_T_[MS_R_WRIST] = cml::MatrixTranslation(p3_[0], p3_[1], p3_[2]);
	calibed_T_[MS_HEAD]    = cml::MatrixTranslation(p4_[0], p4_[1], p4_[2]);
	calibed_T_[MS_PELVIS]  = cml::MatrixTranslation(p5_[0], p5_[1], p5_[2]);

		

	return true;

}

static cml::vector3f ue2cml(FVector v)
{
	return cml::vector3f(v.X, v.Y, v.Z);
}

static FVector cml2ue(cml::vector3f v)
{
	return FVector(v[0], v[1], v[2]);
}

static FVector cml2ue(cml::vector3d v)
{
	return FVector(v[0], v[1], v[2]);
}


static cml::quaterniond ue2cml(FQuat q)
{
	cml::quaterniond cml_q;
	cml_q[cml_q.W] = q.W;
	cml_q[cml_q.X] = q.X;
	cml_q[cml_q.Y] = q.Y;
	cml_q[cml_q.Z] = q.Z;
	return cml_q;
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 시간 받아오기.
	float t = GetGameTimeSinceCreation();
	//p0_.Set(sin(-t)*50.f, cos(-t / 2.f)*50.f, 10.f);
	//p1_.Set(sin(-t)*50.f+100.f, cos(-t/2.f)*50.f, 10.f);

	UWorld *MyWorld = GetWorld();
	cml::matrix33d m_mcr;
	cml::matrix33d m_mcl;
	cml::matrix33d m_hmd, m_tr1, m_tr2, m_tr3;
	if (MyWorld)
	{
		AMyPawnVR* pVR = Cast<AMyPawnVR>(MyWorld->GetFirstPlayerController()->GetPawn());
		//if (pVR->MCROn || pVR->MCLOn)
		{
			// Calibration
			{
				cml::matrix44d cur_user_T[6];

				for (int i = 0; i < 6; i++)
				{
					cur_user_T[i] = cml::MatrixTranslation(ue2cml(pVR->ms_ps[i])) * cml::MatrixRotationQuaternion(ue2cml(pVR->ms_qs[i]));
					calibed_T_[i] = cur_user_T[i] * user_to_data_calib_T_[i];
				}

				
			}
		}
		/*else
		{
			m_mcl = (pVR->mclRotMat);
			m_mcr = (pVR->mcrRotMat);
			m_hmd = (pVR->hmdRotMat);
			m_tr1 = (pVR->tr1RotMat);
			m_tr2 = (pVR->tr2RotMat);
			m_tr3 = (pVR->tr3RotMat);
		}*/
		if (pVR->TelOn)
			SetActorLocation(pVR->ActLoc);

		
	}

	

	UE_LOG(LogTemp, Warning, TEXT("%d"), p0_.X);
	DrawDebugBox(GetWorld(), cml2ue(cml::trans(calibed_T_[MS_L_ANKLE])), FVector(6.f, 6.f, 6.f), FColor(255, 0, 0), false);
	DrawDebugBox(GetWorld(), cml2ue(cml::trans(calibed_T_[MS_R_ANKLE])), FVector(6.f, 6.f, 6.f), FColor(255, 0, 0), false);
	DrawDebugBox(GetWorld(), cml2ue(cml::trans(calibed_T_[MS_L_WRIST])), FVector(6.f, 6.f, 6.f), FColor(0, 255, 0), false);
	DrawDebugBox(GetWorld(), cml2ue(cml::trans(calibed_T_[MS_R_WRIST])), FVector(6.f, 6.f, 6.f), FColor(0, 255, 0), false);
	DrawDebugBox(GetWorld(), cml2ue(cml::trans(calibed_T_[MS_HEAD])), FVector(6.f, 6.f, 6.f), FColor(0, 0, 255), false);
	DrawDebugBox(GetWorld(), cml2ue(cml::trans(calibed_T_[MS_PELVIS])), FVector(6.f, 6.f, 6.f), FColor(0, 0, 0), false);
	

	PBS::SearchResult r;
	PBS::SketchedQuery q;

	if (flag_foot_retarget)
	{
		q.AddJointConstraint(PBS::SketchedQuery::J_LFOT, cml::trans(calibed_T_[MS_L_ANKLE]));
		q.AddJointConstraint(PBS::SketchedQuery::J_RFOT, cml::trans(calibed_T_[MS_R_ANKLE]));
	}

	q.AddJointConstraint(PBS::SketchedQuery::J_LPAM, cml::trans(calibed_T_[MS_L_WRIST]));
	q.AddJointConstraint(PBS::SketchedQuery::J_RPAM, cml::trans(calibed_T_[MS_R_WRIST]));

	if( flag_hmd_retarget )
		q.AddJointConstraint(PBS::SketchedQuery::J_HEAD, cml::trans(calibed_T_[MS_HEAD]));

	if (flag_pelvis_retarget)
		q.AddJointConstraint(PBS::SketchedQuery::J_PELV, cml::trans(calibed_T_[MS_PELVIS]));

	const PBS::MotionDBforPBS *m_db = PBSAppVar::getSingleton()->motino_db();
	
	
	

	if ( m_db->Search(q, r) )
	{
		std::stringstream sstr;
		sstr << r.GetRegisteringTransf();

		UE_LOG(LogTemp, Warning, TEXT("frame, %s, %d, %s"), *FString(r.motion_->name().c_str()), r.frame_, *FString(sstr.str().c_str()));

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("eeeframe"));
	}

	// Retrieved pose

	if (r.motion_)
	{
		ml::Posture pose = r.motion_->posture(r.frame_);

		pose.ApplyTransf(r.GetRegisteringTransf());


		// smooth
		if (flag_smooth)
		{
			if (!flag_first_aftr_reset)
			{
				double w0 = 0.05;
				for (int i = 0; i < pose.body()->num_joint(); i++) {
					pose.rotate(i, cml::slerp(last_pose_data.rotate(i), pose.rotate(i), w0));
				}
				pose.trans(cml::lerp(last_pose_data.trans(), pose.trans(), w0));
			}
		}



		ml::Constraint con;
		if (flag_foot_retarget)
		{
			con.Push(pose.body()->joint_index(ml::L_ANKLE), calibed_T_[MS_L_ANKLE]);
			con.Push(pose.body()->joint_index(ml::R_ANKLE), calibed_T_[MS_R_ANKLE]);
		}

		con.Push(pose.body()->joint_index(ml::L_WRIST), calibed_T_[MS_L_WRIST]);
		con.Push(pose.body()->joint_index(ml::R_WRIST), calibed_T_[MS_R_WRIST]);

		if ( flag_hmd_retarget )
			con.Push(pose.body()->joint_index(ml::HEAD), calibed_T_[MS_HEAD]);

		if (flag_pelvis_retarget)
			con.Push(pose.body()->joint_index(ml::PELVIS), calibed_T_[MS_PELVIS]);



		//pose.IkFullBody(con);
		pose.IkFullBodyAnalytic(con);

		if (flag_display_pose_data)
		{
			for (int i = 0; i < pose.body()->num_joint(); i++)
			{
				FVector p0 = cml2ue(pose.GetGlobalTranslation(i));// r.GetRegisteredPmJoint(i));

				DrawDebugBox(GetWorld(), p0, FVector(2.f, 2.f, 2.f), FColor(0, 0, 255));

				if (i > 0)
				{
					FVector p1 = cml2ue(pose.GetGlobalTranslation(pose.body()->parent(i)));
					DrawDebugLine(GetWorld(), p0, p1, FColor(0, 0, 255), false, -1.f, '\000', 3);
				}
			}
		}



		ml_u_poser_.Retarget(pose);


		last_pose_data = pose;
	}

	if ( false )
	{

		// "JamesPoseable"이 현재 nullptr 라면 여기서 종료.
		if (james_poseable_ == nullptr) return;

		// InitPoseableCharacter() 함수 처리가 실패되었다면 여기서 종료.
		if (flag_valid_poseable_char_ == false) return;

		// 모션 캡쳐 데이터에 내용이 없으면 (frame수가 0이라면) 여기서 종료.
		if (motion_.size()==0) return;


	


		// 시간 t에 해당하는 모션 데이터를 불러와서 'pose' 객체에 담는다.
		ml::Posture pose = motion_.GetPostureAtTime_ms(t);
		ml_u_poser_.Retarget( pose );

	}
	

	flag_first_aftr_reset = false;

}
void AMyActor::ResetHumanT()
{
	flag_first_aftr_reset = true;
	float data_height = 180.0f;

	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		AMyPawnVR* pVR = Cast<AMyPawnVR>(MyWorld->GetFirstPlayerController()->GetPawn());
		
		



		float user_height = p4_[2] - p1_[2];
		float user_to_data_scale = data_height / user_height;
		cml::vector3 user_ground_center = ue2cml(pVR->ms_ps[MS_L_ANKLE] + pVR->ms_ps[MS_R_ANKLE]) / 2.0;

		if (!flag_foot_retarget)
		{
			user_ground_center = ue2cml(pVR->ms_ps[MS_L_WRIST] + pVR->ms_ps[MS_R_WRIST]) / 2.0;
		}

		cml::matrix44d user_r;
		cml::vector3d r2l = ue2cml(pVR->ms_ps[MS_L_WRIST] - pVR->ms_ps[MS_R_WRIST]);
		r2l[2] = 0.;
		cml::matrix_rotation_vec_to_vec(user_r, cml::vector3d(1., 0., 0.), cml::normalize(r2l));

		user_to_data_scale_t_ = 
			cml::MatrixTranslation((cml::vector3d)(user_ground_center))
		* cml::MatrixUniformScaling(user_to_data_scale)
		* cml::MatrixTranslation( (cml::vector3d)(-user_ground_center) );

		// ground to waist 103.8
		if (PBSAppVar::getSingleton()->motino_db()->size() > 0)
		{
			ml::Posture t_pose = PBSAppVar::getSingleton()->motino_db()->getMotion(0)->posture(0);
			t_pose.SetIdentityPose();
			t_pose.SetGlobalTrans(cml::vector3d(user_ground_center[0], user_ground_center[1], 103.8));
			t_pose.SetGlobalRotation(0, cml::mat3(user_r));

			for (int i = 0; i < t_pose.body()->num_joint(); i++)
			{
				FVector p0 = cml2ue(t_pose.GetGlobalTranslation(i));// r.GetRegisteredPmJoint(i));

				DrawDebugBox(GetWorld(), p0, FVector(2.f, 2.f, 2.f), FColor(255, 0, 0), false, 10.f);

				if (i > 0)
				{
					FVector p1 = cml2ue(t_pose.GetGlobalTranslation(t_pose.body()->parent(i)));
					DrawDebugLine(GetWorld(), p0, p1, FColor(255, 0, 0), false, 10.f, '\000', 3);
				}
			}

			cml::matrix44d data_Ts[6];
			data_Ts[MS_HEAD]    = t_pose.GetGlobalTransf(ml::HEAD);
			data_Ts[MS_L_WRIST] = t_pose.GetGlobalTransf(ml::L_WRIST);
			data_Ts[MS_R_WRIST] = t_pose.GetGlobalTransf(ml::R_WRIST);
			data_Ts[MS_L_ANKLE] = t_pose.GetGlobalTransf(ml::L_ANKLE);
			data_Ts[MS_R_ANKLE] = t_pose.GetGlobalTransf(ml::R_ANKLE);
			data_Ts[MS_PELVIS]  = t_pose.GetGlobalTransf(ml::PELVIS);

			cml::matrix44d user_Ts[6];
			for (int i = 0; i < 6; i++)
			{
				user_Ts[i] = cml::MatrixTranslation(ue2cml(pVR->ms_ps[i])) * cml::MatrixRotationQuaternion(ue2cml(pVR->ms_qs[i]));
				user_to_data_calib_T_[i] = cml::inverse(user_Ts[i]) * data_Ts[i];
			}


			/*for (auto &d : user_to_data_calib_T_)
			{
				d = cml::MatrixTranslation(0., 0., 0.);
			}*/



		}
	}
}


bool AMyActor::GrapJointByHand(FVector joint, FVector hand)
{
	if (sqrt((joint.X - hand.X)*(joint.X - hand.X) + (joint.Y - hand.Y)*(joint.Y - hand.Y) + 
		(joint.Z - hand.Z)*(joint.Z - hand.Z)) < 5.f)
		return true;
	return false;
}

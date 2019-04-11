// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"
#include "Misc/Paths.h"				// FPaths::ProjectContentDir() ����� ����...
#include "EngineUtils.h"			// Debug ����� ����...
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
	//// 1. UE4 ĳ���� �� �غ�
	//// 1.1 InitPoseableCharacter() �Լ��� ȣ�� 
	// BluePrinter�� ���� ������ ĳ���� ���� c++ code���� ������ �� �ֵ��� �غ��Ѵ�.
	// ��ü������ ���ϸ� ��� ������ 'poseable_char_'�� 'ml_u_posers_'�� ��� ������ ���·� �غ��Ѵ�.
	// �Լ��� ���������� ó���� ��� true�� ���ϵȴ�.
	// �ݵ�� InitPoseableCharacter() �Լ��� ���뵵 ���캸��. 
	flag_valid_poseable_char_ = InitPoseableCharacter();




	//////////////////////////////////////////////////////////////////////////////////////////
	//// 2. ��� ĸ�� ������ �ε�
	//// 2.1 ��� ������ ������ ��ġ(����)�� �̸� ����
	// FPaths::ProjectContentDir() �� UE4 ������Ʈ ������ Content ���� ��ġ�� �����Ѵ�.
	FString T_pose_filename = FPaths::ProjectContentDir() + "bvh/bsh_T.bvh";	// T-Pose �ڼ��� �����ϰ��ִ� ��� ������ ����
	FString motion_filename = FPaths::ProjectContentDir() + "bvh/bsh_dance.bvh"; // �ε��ؾ��� ��� ������ ����


	//// 2.2 'motion_filename' �ε�
	// ��� ������ ������ �ε��ϰ� �� ������ 'motion_' ��ü�� ��´�.
	// �ݵ�� T-Pose �ڼ��� �����ϰ� �ִ� ������ ������ ù��° �Ķ���ͷ� �־���Ѵ�.
	// ML libaray������ T-Pose�� �������� ������� ��ü �ڼ��� ��ȭ�� �м��ϰ� ���� ��� ��꿡 ����Ѵ�.
	ml::LoadBVH_UE4(T_pose_filename, motion_filename, motion_);

	const PBS::MotionDBforPBS *m_db = PBSAppVar::getSingleton()->motino_db();
	//m_db->getMotion(0)->Clone(&motion_);

	//// 2.3 ��� ������ �� ��ü ���� �̸� ����
	// ������ ��� ĸ�� �����ʹ� ���ϸ��� ��ü ������ ���� �̸��� ���� �ٸ��� ��ϵǾ����� ���̴�.
	// ML library�� ����ϱ� ���ؼ��� �� ��ü ������ ������ ��ӵ� �̸�(JointTag)�� �ο��Ͽ��� �Ѵ�.
	// SetJointTag("��� ĸ�� ������ ���� �̸�", "ML library�� ���� �� �̸�").
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
	//// 1. 'james_posable_' ����
	//// 1.1 ���� Actor�� �����ϰ� �ִ� ��� UPoseableMeshComponent ��ü�� �����͸� �޾Ƽ�,
	////     'components' �迭�� ��´�. 
	TArray<UPoseableMeshComponent*> components;
	GetComponents(components);

	//// 1.2 components �迭�� �˻��Ͽ� �̸��� "JamePoseable" �� ���� ã��,
	////     �� �����͸� �ɹ� ���� 'jame_poseable_' �� �����Ѵ�.
	james_poseable_ = nullptr;
	for (int i = 0; i<components.Num(); i++)
	{
		if (components[i]->GetName() == "JamesPoseable")
		{
			james_poseable_ = (UPoseableMeshComponent*)components[i];
			// UE_LOG(LogTemp, Warning, TEXT("james_poseable_ = %s"), *(james_poseable_->GetName())); // ����� �� ���
		}
	}



	// "JamesPoseable" ã�⿡ ������ ��� return false.
	if (james_poseable_ == nullptr) return false;	

	// "JamesPoseable" �� ������ �޽� ���� �����ϰ� ���� ���� ��� return false.
	if (james_poseable_->SkeletalMesh == nullptr) return false;	




	//////////////////////////////////////////////////////////////////////////////////////////
	//// 2. 'ml_u_poser_' ����
	//// 2.1 'james_poseable_' ������ ���� �Է��Ѵ�.
	ml_u_poser_.SetUPoseableMeshComponent(james_poseable_);

	//// 2.2 ĳ���� �� ��ü ���� �̸� ����
	// ������ ĳ���� ���� ��ü ���� �̸��� �����ڰ� ���Ƿ� �ο��ϱ� ������ ���� �ٸ� ���̴�.
	// ML library�� ����ϱ� ���ؼ��� �� ��ü ������ ������ ��ӵ� �̸�(JointTag)�� �ο��Ͽ��� �Ѵ�.
	// SetJointTag("ĳ���� �� ���� ��ü �̸�", "ML library�� ���� �� �̸�").
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

	//// 2.3 ���� ������ ������ �������� 'ml_u_poser' ���� �����͸� ������Ʈ�Ѵ�.
	// 'BuildSkeleton()' �Լ��� �ݵ�� 2.1�� 2.2�� ������ ������ ����Ǿ���Ѵ�.
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
	
	// �ð� �޾ƿ���.
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

		// "JamesPoseable"�� ���� nullptr ��� ���⼭ ����.
		if (james_poseable_ == nullptr) return;

		// InitPoseableCharacter() �Լ� ó���� ���еǾ��ٸ� ���⼭ ����.
		if (flag_valid_poseable_char_ == false) return;

		// ��� ĸ�� �����Ϳ� ������ ������ (frame���� 0�̶��) ���⼭ ����.
		if (motion_.size()==0) return;


	


		// �ð� t�� �ش��ϴ� ��� �����͸� �ҷ��ͼ� 'pose' ��ü�� ��´�.
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

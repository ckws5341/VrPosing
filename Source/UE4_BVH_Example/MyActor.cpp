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

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	
}

AMyActor::~AMyActor()
{
	PBSAppVar::DestroySingleton();
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();


	




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

	if (MyWorld)
	{
		AMyPawnVR* pVR = Cast<AMyPawnVR>(MyWorld->GetFirstPlayerController()->GetPawn());
		if (pVR->MCROn || pVR->MCLOn)
		{
			/*if (pVR->MCROn)
			{
				if (GrapJointByHand(p0_, pVR->p1))
					p0_ = pVR->p1;
				else if (GrapJointByHand(p1_, pVR->p1))
					p1_ = pVR->p1;
				else if (GrapJointByHand(p2_, pVR->p1))
					p2_ = pVR->p1;
				else if (GrapJointByHand(p3_, pVR->p1))
					p3_ = pVR->p1;
				else if (GrapJointByHand(p4_, pVR->p1))
					p4_ = pVR->p1;
				else if (GrapJointByHand(p5_, pVR->p1))
					p5_ = pVR->p1;
			}
			if (pVR->MCLOn)
			{
				if (GrapJointByHand(p0_, pVR->p2))
					p0_ = pVR->p2;
				else if (GrapJointByHand(p1_, pVR->p2))
					p1_ = pVR->p2;
				else if (GrapJointByHand(p2_, pVR->p2))
					p2_ = pVR->p2;
				else if (GrapJointByHand(p3_, pVR->p2))
					p3_ = pVR->p2;
				else if (GrapJointByHand(p4_, pVR->p2))
					p4_ = pVR->p2;
				else if (GrapJointByHand(p5_, pVR->p2))
					p5_ = pVR->p2;
			}*/
			p0_ =pVR->d4;
			p1_ =pVR->d5; // Foot

			p2_ = pVR->d2; 
			p3_ = pVR->d1; // Hand

			p4_ = pVR->d3; // Head
		
			p5_ = pVR->d6; // Pelvis
		}
		if (pVR->TelOn)
			SetActorLocation(pVR->ActLoc);

		m_mcl = (pVR->mclRotMat);
		m_mcr = (pVR->mcrRotMat);
	}
	UE_LOG(LogTemp, Warning, TEXT("%d"), p0_.X);
	DrawDebugBox(GetWorld(), p1_, FVector(6.f, 6.f, 6.f), FColor(255, 0, 0), false);
	DrawDebugBox(GetWorld(), p0_, FVector(6.f, 6.f, 6.f), FColor(255, 0, 0), false);
	DrawDebugBox(GetWorld(), p2_, FVector(6.f, 6.f, 6.f), FColor(0, 255, 0), false);
	DrawDebugBox(GetWorld(), p3_, FVector(6.f, 6.f, 6.f), FColor(0, 255, 0), false);
	DrawDebugBox(GetWorld(), p4_, FVector(6.f, 6.f, 6.f), FColor(0, 0, 255), false);
	DrawDebugBox(GetWorld(), p5_, FVector(6.f, 6.f, 6.f), FColor(0, 0, 0), false);
	

	PBS::SearchResult r;
	PBS::SketchedQuery q;
	//q.AddJointConstraint(PBS::SketchedQuery::J_LFOT, ue2cml( p0_) );
	//q.AddJointConstraint(PBS::SketchedQuery::J_RFOT, ue2cml( p1_) );

	q.AddJointConstraint(PBS::SketchedQuery::J_LPAM, ue2cml(p2_));
	q.AddJointConstraint(PBS::SketchedQuery::J_RPAM, ue2cml(p3_));

	//q.AddJointConstraint(PBS::SketchedQuery::J_HEAD, ue2cml(p4_));
	//q.AddJointConstraint(PBS::SketchedQuery::J_PELV, ue2cml(p5_));

	const PBS::MotionDBforPBS *m_db = PBSAppVar::getSingleton()->motino_db();
	
	
	

	if ( m_db->Search(q, r) )
	{
		UE_LOG(LogTemp, Warning, TEXT("frame, %s, %d"), *FString(r.motion_->name().c_str()), r.frame_);
	}
	else
	{	
		UE_LOG(LogTemp, Warning, TEXT("eeeframe"));
	} 

	// Retrieved pose

	if ( r.motion_ )
	{


		ml::Posture pose = r.motion_->posture(r.frame_);
		
		pose.ApplyTransf(r.GetRegisteringTransf());


		if (false&&last_pose_data.body())
		{
			double w0 = 0.1;
			double w1 = 0.9;
			for (int i = 1; i < pose.body()->num_joint(); i++) {
				pose.rotate(i, cml::slerp(last_pose_data.rotate(i), pose.rotate(i), w0));
			}
			//pose.trans(cml::vector3d(pose.trans()[0], cml::lerp(last_pose_data.trans()[1], pose.trans()[1], w0), pose.trans()[2]));
			//pose.trans(cml::lerp(last_pose_data.trans(), pose.trans(), w0));

		}


		
		ml::Constraint con;
		con.Push(pose.body()->joint_index(ml::L_ANKLE), ue2cml(p0_));
		con.Push(pose.body()->joint_index(ml::R_ANKLE), ue2cml(p1_));
		con.Push(pose.body()->joint_index(ml::L_WRIST), ue2cml(p2_));
		con.Push(pose.body()->joint_index(ml::R_WRIST), ue2cml(p3_));
		con.Push(pose.body()->joint_index(ml::HEAD), ue2cml(p4_));
		con.Push(pose.body()->joint_index(ml::PELVIS), ue2cml(p5_));

		con.Push(pose.body()->joint_index(ml::L_WRIST), m_mcl);//���Ⱚ �Է�(matrix)
		con.Push(pose.body()->joint_index(ml::R_WRIST), m_mcr);
		//pose.IkFullBody(con);
		//pose.IkFullBodyAnalytic(con);

		for ( int i=0; i<pose.body()->num_joint(); i++ )
		{
			FVector p0 = cml2ue(pose.GetGlobalTranslation(i));// r.GetRegisteredPmJoint(i));
			
			DrawDebugBox(GetWorld(), p0, FVector(2.f, 2.f, 2.f), FColor(0, 0, 255));

			if ( i>0 )
			{
				FVector p1 = cml2ue(pose.GetGlobalTranslation(pose.body()->parent(i)));
				DrawDebugLine(GetWorld(), p0, p1, FColor(0, 0, 255), false, -1.f, '\000', 3);
			}
		}

		
		
		ml_u_poser_.Retarget( pose );


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
	
}
bool AMyActor::GrapJointByHand(FVector joint, FVector hand)
{
	if (sqrt((joint.X - hand.X)*(joint.X - hand.X) + (joint.Y - hand.Y)*(joint.Y - hand.Y) + 
		(joint.Z - hand.Z)*(joint.Z - hand.Z)) < 5.f)
		return true;
	return false;
}

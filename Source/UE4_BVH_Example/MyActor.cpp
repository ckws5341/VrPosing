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
	ml_u_poser_.SetJointTag("pelvis", ml::PELVIS);
	ml_u_poser_.SetJointTag("spine_01", ml::SPINE);
	ml_u_poser_.SetJointTag("spine_02", ml::SPINE1);
	ml_u_poser_.SetJointTag("spine_03", ml::CHEST);
	ml_u_poser_.SetJointTag("neck_01", ml::NECK);
	ml_u_poser_.SetJointTag("head", ml::HEAD);

	ml_u_poser_.SetJointTag("thigh_r", ml::R_HIP);
	ml_u_poser_.SetJointTag("calf_r", ml::R_KNEE);
	ml_u_poser_.SetJointTag("foot_r", ml::R_ANKLE);
	ml_u_poser_.SetJointTag("ball_r", ml::R_FOOT);

	ml_u_poser_.SetJointTag("thigh_l", ml::L_HIP);
	ml_u_poser_.SetJointTag("calf_l", ml::L_KNEE);
	ml_u_poser_.SetJointTag("foot_l", ml::L_ANKLE);
	ml_u_poser_.SetJointTag("ball_l", ml::L_FOOT);
	

	ml_u_poser_.SetJointTag("clavicle_r", ml::R_CLAVICLE);
	ml_u_poser_.SetJointTag("upperarm_r", ml::R_SHOULDER);
	ml_u_poser_.SetJointTag("lowerarm_r", ml::R_ELBOW);
	ml_u_poser_.SetJointTag("hand_r", ml::R_WRIST);

	ml_u_poser_.SetJointTag("clavicle_l", ml::L_CLAVICLE);
	ml_u_poser_.SetJointTag("upperarm_l", ml::L_SHOULDER);
	ml_u_poser_.SetJointTag("lowerarm_l", ml::L_ELBOW);
	ml_u_poser_.SetJointTag("hand_l", ml::L_WRIST);


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
	
	if (MyWorld)
	{
		AMyPawnVR* pVR = Cast<AMyPawnVR>(MyWorld->GetFirstPlayerController()->GetPawn());
		if (pVR)
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
		}
		if (pVR->TelOn)
			SetActorLocation(pVR->ActLoc);
	}
	UE_LOG(LogTemp, Warning, TEXT("%d"), p0_.X);
	DrawDebugSphere(GetWorld(), p1_, 10, 16, FColor(255, 0, 0), false);
	DrawDebugSphere(GetWorld(), p0_, 10, 16, FColor(0, 255, 0), false);
	DrawDebugSphere(GetWorld(), p2_, 10, 16, FColor(255, 0, 0), false);
	DrawDebugSphere(GetWorld(), p3_, 10, 16, FColor(0, 255, 0), false);
	DrawDebugSphere(GetWorld(), p4_, 10, 16, FColor(255, 0, 0), false);

	PBS::SearchResult r;
	PBS::SketchedQuery q;
	q.AddJointConstraint(PBS::SketchedQuery::J_LFOT, ue2cml( p0_) );
	q.AddJointConstraint(PBS::SketchedQuery::J_RFOT, ue2cml( p1_) );

	q.AddJointConstraint(PBS::SketchedQuery::J_LPAM, ue2cml(p2_));
	q.AddJointConstraint(PBS::SketchedQuery::J_RPAM, ue2cml(p3_));

	q.AddJointConstraint(PBS::SketchedQuery::J_HEAD, ue2cml(p4_));
	
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

		ml::Constraint con;
		con.Push(pose.body()->joint_index(ml::L_ANKLE), ue2cml(p0_));
		con.Push(pose.body()->joint_index(ml::R_ANKLE), ue2cml(p1_));
		con.Push(pose.body()->joint_index(ml::L_WRIST), ue2cml(p2_));
		con.Push(pose.body()->joint_index(ml::R_WRIST), ue2cml(p3_));
		con.Push(pose.body()->joint_index(ml::HEAD ), ue2cml(p4_));
		pose.IkFullBody(con);

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

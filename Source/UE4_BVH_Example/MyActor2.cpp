// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor2.h"
#include "Misc/Paths.h"				// FPaths::ProjectContentDir() ����� ����...
#include "EngineUtils.h"			// Debug ����� ����...

// Sets default values
AMyActor2::AMyActor2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor2::BeginPlay()
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
	//// 2. ���� ��� ĸ�� ������ �ε�
	//// 2.1 ��� ������ ������ ��ġ(����)�� �̸� ����
	// FPaths::ProjectContentDir() �� UE4 ������Ʈ ������ Content ���� ��ġ�� �����Ѵ�.
	FString T_pose_filename = FPaths::ProjectContentDir() + "bvh/bsh_T.bvh";	// T-Pose �ڼ��� �����ϰ��ִ� ��� ������ ����
	FString run_motion_filename = FPaths::ProjectContentDir() + "bvh/ljh_Run001.bvh"; // �ε��ؾ��� ��� ������ ����
	FString wing_motion_filename = FPaths::ProjectContentDir() + "bvh/kyc_wing.bvh"; // �ε��ؾ��� ��� ������ ����

	//// 2.2 run_motion ��ü�� ljh_Run001.bvh�� �ε��Ѵ�.
	// �ݵ�� T-Pose �ڼ��� �����ϰ� �ִ� ������ ������ ù ��° �Ķ���ͷ� �־���Ѵ�.
	// ML libaray������ T-Pose�� �������� ������� ��ü �ڼ��� ��ȭ�� �м��ϰ� ���� ��� ��꿡 ����Ѵ�.
	ml::Motion run_motion;
	ml::LoadBVH_UE4(T_pose_filename, run_motion_filename, run_motion);

	//// 2.3 wing_motion ��ü�� ljh_Run001.bvh�� �ε��Ѵ�.
	// �ݵ�� T-Pose �ڼ��� �����ϰ� �ִ� ������ ������ ù ��° �Ķ���ͷ� �־���Ѵ�.
	// ML libaray������ T-Pose�� �������� ������� ��ü �ڼ��� ��ȭ�� �м��ϰ� ���� ��� ��꿡 ����Ѵ�.
	ml::Motion wing_motion;
	ml::LoadBVH_UE4(T_pose_filename, wing_motion_filename, wing_motion);






	//////////////////////////////////////////////////////////////////////////////////////////
	//// 3. ��� ĸ�� ������ ����
	//// 3.1 �޸��� �κ� �߶󳻱�.
	// 'run_motion'�� ��ü ���� �� 30�� �����Ӻ��� 20���� �߶� 'run_onecylce_motion' ��ü�� ������ �ִ´�.
	ml::Motion run_onecylce_motion;
	run_motion.CropMotion(30, 20, &run_onecylce_motion);

	//// 3.2 �޸��� ���� �κ� �߶󳻱�
	// 'run_motion'�� ��ü ���� �� 270�� �����Ӻ��� 60���� �߶� 'stop_motion' ��ü�� ������ �ִ´�.
	ml::Motion stop_motion;
	run_motion.CropMotion(270, 60, &stop_motion);

	//// 3.3 ������ ���߱� �߶󳻱�
	// 'wing_motion'�� ��ü ���� �� 66�� �����Ӻ��� 100���� �߶� 'wing_motion_01' ��ü�� ������ �ִ´�.
	ml::Motion wing_motion_01;
	wing_motion.CropMotion(66, 100, &wing_motion_01);

	//// 3.4 �̾� ���̱�.
	// �߶� ��� ������ 'motion_' ��ü�� ������ �̾���δ�.
	// ���ο� �ϳ��� ��� �����͸� �����.
	motion_ = run_onecylce_motion;				// ù �κ� ���� (�޸���)
	motion_.Stitch_UE4(run_onecylce_motion);	// ���� �κ� �̾���̱� (�޸��� �ݺ�)
	motion_.Stitch_UE4(run_onecylce_motion);	// ���� �κ� �̾���̱� (�޸��� �ݺ�)
	motion_.Stitch_UE4(run_onecylce_motion);	// ���� �κ� �̾���̱� (�޸��� �ݺ�)
	motion_.Stitch_UE4(stop_motion);			// ���� �κ� �̾���̱� (����)
	motion_.Stitch_UE4(wing_motion_01);			// ���� �κ� �̾���̱� (���� ��)
	motion_.Stitch_UE4(wing_motion_01);			// ���� �κ� �̾���̱� (���� ��)
	motion_.Stitch_UE4(wing_motion_01);			// ���� �κ� �̾���̱� (���� ��)

	





	//////////////////////////////////////////////////////////////////////////////////////////
	//// 4. �ϼ��� ��� ������ �� ��ü ���� �̸� ����
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

bool AMyActor2::InitPoseableCharacter()
{
	//////////////////////////////////////////////////////////////////////////////////////////
	//// 1. 'tom_poseable_' ����
	//// 1.1 ���� Actor�� �����ϰ� �ִ� ��� UPoseableMeshComponent ��ü�� �����͸� �޾Ƽ�,
	////     'components' �迭�� ��´�. 
	TArray<UPoseableMeshComponent*> components;
	GetComponents(components);

	//// 1.2 components �迭�� �˻��Ͽ� �̸��� "JamePoseable" �� ���� ã��,
	////     �� �����͸� �ɹ� ���� 'jame_poseable_' �� �����Ѵ�.
	tom_poseable_ = nullptr;
	for (int i = 0; i<components.Num(); i++)
	{
		if (components[i]->GetName() == "TomPoseable")
		{
			tom_poseable_ = (UPoseableMeshComponent*)components[i];
			// UE_LOG(LogTemp, Warning, TEXT("tom_poseable_ = %s"), *(tom_poseable_->GetName())); // ����� �� ���
		}
	}



	// "TomPoseable" ã�⿡ ������ ��� return false.
	if (tom_poseable_ == nullptr) return false;	

	// "TomPoseable" �� ������ �޽� ���� �����ϰ� ���� ���� ��� return false.
	if (tom_poseable_->SkeletalMesh == nullptr) return false;	




	//////////////////////////////////////////////////////////////////////////////////////////
	//// 2. 'ml_u_poser_' ����
	//// 2.1 'tom_poseable_' ������ ���� �Է��Ѵ�.
	ml_u_poser_.SetUPoseableMeshComponent(tom_poseable_);

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

// Called every frame
void AMyActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// "JamesPoseable"�� ���� nullptr ��� ���⼭ ����.
	if (tom_poseable_ == nullptr) return;

	// InitPoseableCharacter() �Լ� ó���� ���еǾ��ٸ� ���⼭ ����.
	if (flag_valid_poseable_char_ == false) return;

	// ��� ĸ�� �����Ϳ� ������ ������ (frame���� 0�̶��) ���⼭ ����.
	if (motion_.size()==0) return;


	// �ð� �޾ƿ���.
	float t = GetGameTimeSinceCreation();


	// �ð� t�� �ش��ϴ� ��� �����͸� �ҷ��ͼ� 'pose' ��ü�� ��´�.
	ml::Posture pose = motion_.GetPostureAtTime_ms(t);
	ml_u_poser_.Retarget( pose );

}


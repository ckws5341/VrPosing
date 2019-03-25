// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PoseableMeshComponent.h"	// UPoseableMeshComponent Ŭ���� ����� ����..
#include "Motion/ml.h"							// ��ǵ����� ó�� ���̺귯�� (ML library) ����� ����..
#include "MyActor.generated.h"

UCLASS()
class UE4_BVH_EXAMPLE_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();
	virtual ~AMyActor();
	FVector p0_ = { 15.f, 0.f, -5.f }; //left ankle
	FVector p1_ = { -15.f, 0.f, -5.f }; // right ankle
	FVector p2_ = { 80.f, -10.f, 120.f }; // left wrist
	FVector p3_ = { -80.f, -10.f, 120.f }; // right wrist
	FVector p4_ = { 0.f, 15.f, 170.f }; // head
	FVector p5_ = { 0.f, 0.f, 90.f }; // pelvis

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	ĳ���� ���� �ڼ� ��꿡 �ʿ��� ������ �غ��ϴ� �Լ�.
	�Լ��� ���������� ó���Ǵ� ��� poseable_char_(������ ����)�� ml_u_posers_(��ü ����)�� ��� ������ ���·� �غ�ȴ�.
	*/
	bool InitPoseableCharacter();

	bool flag_first_aftr_reset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ResetHumanT();

	
	UPoseableMeshComponent *james_poseable_;	// ĳ���� �� ��ü�� ���� ������
	bool flag_valid_poseable_char_;				// ĳ���� ���� ���������� �ε�Ǿ����� Ȯ���ϴ� flag ����
	
	ml::Motion motion_;							// ��� ĸ�� ������ 

	ml::UE4Poser ml_u_poser_;					// ��� ĸ�� �����͸� UE4 ĳ���� �𵨿� �����ϴ� ������ ó���ϴ� ��ü

	bool GrapJointByHand(FVector joint, FVector hand);

	cml::matrix44d user_to_data_scale_t_;

	enum MS_NAME { MS_PELVIS = 0, MS_HEAD, MS_L_WRIST, MS_R_WRIST, MS_L_ANKLE, MS_R_ANKLE };
	cml::matrix44d user_to_data_calib_T_[6];
	cml::matrix44d calibed_T_[6];

	ml::Posture last_pose_data;

	UPROPERTY(EditAnywhere, Category="MyActor Op", BlueprintReadWrite)
	bool flag_smooth;

	UPROPERTY(EditAnywhere, Category = "MyActor Op", BlueprintReadWrite)
	bool flag_hmd_retarget;
	UPROPERTY(EditAnywhere, Category = "MyActor Op", BlueprintReadWrite)
	bool flag_pelvis_retarget;
	UPROPERTY(EditAnywhere, Category = "MyActor Op", BlueprintReadWrite)
	bool flag_foot_retarget;
	UPROPERTY(EditAnywhere, Category = "MyActor Op", BlueprintReadWrite)
	bool flag_display_pose_data;

};

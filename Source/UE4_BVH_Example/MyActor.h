// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PoseableMeshComponent.h"	// UPoseableMeshComponent 클래스 사용을 위해..
#include "Motion/ml.h"							// 모션데이터 처리 라이브러리 (ML library) 사용을 위해..
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
	캐릭터 모델의 자세 계산에 필요한 정보를 준비하는 함수.
	함수가 성공적으로 처리되는 경우 poseable_char_(포인터 변수)와 ml_u_posers_(객체 변수)가 사용 가능한 상태로 준비된다.
	*/
	bool InitPoseableCharacter();

	bool flag_first_aftr_reset;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ResetHumanT();

	
	UPoseableMeshComponent *james_poseable_;	// 캐릭터 모델 객체에 대한 포인터
	bool flag_valid_poseable_char_;				// 캐릭터 모델이 정상적으로 로드되었는지 확인하는 flag 변수
	
	ml::Motion motion_;							// 모션 캡쳐 데이터 

	ml::UE4Poser ml_u_poser_;					// 모션 캡쳐 데이터를 UE4 캐릭터 모델에 적용하는 절차를 처리하는 객체

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

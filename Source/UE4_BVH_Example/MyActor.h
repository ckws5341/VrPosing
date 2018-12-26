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
	FVector p0_ = { 1.f, 0.f, 0.f };
	FVector p1_ = { -1.f, 0.f, 0.f };


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	ĳ���� ���� �ڼ� ��꿡 �ʿ��� ������ �غ��ϴ� �Լ�.
	�Լ��� ���������� ó���Ǵ� ��� poseable_char_(������ ����)�� ml_u_posers_(��ü ����)�� ��� ������ ���·� �غ�ȴ�.
	*/
	bool InitPoseableCharacter();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UPoseableMeshComponent *james_poseable_;	// ĳ���� �� ��ü�� ���� ������
	bool flag_valid_poseable_char_;				// ĳ���� ���� ���������� �ε�Ǿ����� Ȯ���ϴ� flag ����
	
	ml::Motion motion_;							// ��� ĸ�� ������ 

	ml::UE4Poser ml_u_poser_;					// ��� ĸ�� �����͸� UE4 ĳ���� �𵨿� �����ϴ� ������ ó���ϴ� ��ü


	
};

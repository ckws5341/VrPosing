// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "XRMotionControllerBase.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "MyActor.h"
#include "UserHandActorVR.h"
#include "EngineUtils.h"
#include "MyPawnVR.generated.h"

UCLASS()
class UE4_BVH_EXAMPLE_API AMyPawnVR : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawnVR();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere) USceneComponent * CameraRootComponent;

	UPROPERTY(EditAnywhere) UCameraComponent * VRCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * MCR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * MCL;

public:
	FVector p1 = { 1.f, 0.f, 0.f };
	FVector p2 = { -1.f, 0.f, 0.f };
	bool MCROn, MCLOn = false;
	bool trackpadOn = false;
	void Input_MCR_TriggerAction_DOWN() {
		MCROn = true;
	}
	void Input_MCR_TriggerAction_UP() {
		MCROn = false;
	}
	void Input_MCL_TriggerAction_DOWN() {
		MCLOn = true;
	}
	void Input_MCL_TriggerAction_UP() {
		MCLOn = false;
	}
	//void Input_MCR_Trackpad1_DOWN();
	//void Input_MCR_Trackpad1_UP();
};

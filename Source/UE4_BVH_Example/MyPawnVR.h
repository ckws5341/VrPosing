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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * HMD;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * MCR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * MCL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * TR1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * TR2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UMotionControllerComponent * TR3;
public:
	FVector p1, p2, p3, p4, p5, p6;
	FVector d1, d2, d3, d4, d5, d6, temp;
	bool MCROn, MCLOn, TelOn= false;
	bool trackpadOn = false;
	cml::matrix33d mcrRotMat, mclRotMat, hmdRotMat;
	cml::matrix33d tr1RotMat, tr2RotMat, tr3RotMat;
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
	void Input_MCL_ShoulderAction_DOWN();
	void Input_MCL_ShoulderAction_UP();


	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void Move_ZAxis(float AxisValue);

	cml::matrix33d CalRotation(FQuat q);
	FVector CurrentVelocity;
	FVector ActLoc;
};

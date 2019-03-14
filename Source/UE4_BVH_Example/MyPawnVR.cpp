// Fill out your copyright notice in the Description page of Project Settings.
#include "MyPawnVR.h"
#include "Engine.h"
#include "Components/InputComponent.h"
// Sets default values
AMyPawnVR::AMyPawnVR()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CameraRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRootComponent->SetupAttachment(RootComponent);
	CameraRootComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	VRCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCameraComponent->SetupAttachment(CameraRootComponent);
	VRCameraComponent->SetRelativeLocation(FVector(0.0f, 60.0f, 60.0f));
	VRCameraComponent->bLockToHmd = false;

	//VRCamera2Component = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera2"));
	//VRCamera2Component->SetRelativeLocation(FVector(0.0f, 60.0f, 60.0f));

	HMD = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("HeadMountDisplay"));
	HMD->SetupAttachment(CameraRootComponent);
	HMD->MotionSource = FXRMotionControllerBase::HMDSourceId;

	MCR = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_R"));
	MCR->SetupAttachment(CameraRootComponent);
	MCR->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	MCR->Hand_DEPRECATED = EControllerHand::Right;

	MCL = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_L"));
	MCL->SetupAttachment(CameraRootComponent);
	MCL->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	MCL->Hand_DEPRECATED = EControllerHand::Left;
	
	TR1 = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Tracker1"));
	TR1->SetupAttachment(CameraRootComponent);
	//TR1->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	TR1->Hand_DEPRECATED = EControllerHand::Special_1;

	TR2 = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Tracker2"));
	TR2->SetupAttachment(CameraRootComponent);
	//TR2->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	TR2->Hand_DEPRECATED = EControllerHand::Special_2;

	TR3 = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Tracker3"));
	TR3->SetupAttachment(CameraRootComponent);
	//TR3->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	TR3->Hand_DEPRECATED = EControllerHand::Special_3;

}

// Called when the game starts or when spawned
void AMyPawnVR::BeginPlay()
{
	Super::BeginPlay();
	p1, p2, p3, p4, p5, p6 = { 0.f, 1.f, 0.f };
}

// Called every frame
void AMyPawnVR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MCR->Activate(true);
	MCL->Activate(true);
	UWorld *MyWorld = GetWorld();
	temp, d1, d2, d3, d4, d5, d6 = { 0.f, 0.f, 0.f };
	FVector mid_ = { -70.f, 280.f, 250.f };
	FVector inv_ = { -1.f, -1.f, 1.f };

	FRotator mcrRot = MCR->GetComponentRotation();
	FRotator mclRot = MCL->GetComponentRotation();
	FRotator hmdRot = HMD->GetComponentRotation();
	FRotator tr1Rot = TR1->GetComponentRotation();
	FRotator tr2Rot = TR2->GetComponentRotation();
	FRotator tr3Rot = TR3->GetComponentRotation();

	cml::matrix33d mcrRotRollMat;
	mcrRotRollMat(0, 0) = cos(mcrRot.Roll); mcrRotRollMat(1, 0) = -sin(mcrRot.Roll); mcrRotRollMat(2, 0) = 0;
	mcrRotRollMat(0, 1) = sin(mcrRot.Roll); mcrRotRollMat(1, 1) = cos(mcrRot.Roll); mcrRotRollMat(2, 1) = 0;
	mcrRotRollMat(0, 2) = 0; mcrRotRollMat(1, 2) = 0; mcrRotRollMat(2, 2) = 1;

	cml::matrix33d mcrRotPitchMat;
	mcrRotPitchMat(0, 0) = 1; mcrRotPitchMat(1, 0) = 0; mcrRotPitchMat(2, 0) = 0;
	mcrRotPitchMat(0, 1) = 0; mcrRotPitchMat(1, 1) = cos(mcrRot.Pitch); mcrRotPitchMat(2, 1) = -sin(mcrRot.Pitch);
	mcrRotPitchMat(0, 2) = 0; mcrRotPitchMat(1, 2) = sin(mcrRot.Pitch); mcrRotPitchMat(2, 2) = cos(mcrRot.Pitch);

	cml::matrix33d mcrRotYawMat;
	mcrRotYawMat(0, 0) = cos(mcrRot.Yaw); mcrRotYawMat(1, 0) = 0; mcrRotYawMat(2, 0) = sin(mcrRot.Yaw);
	mcrRotYawMat(0, 1) = 0; mcrRotYawMat(1, 1) = 1; mcrRotYawMat(2, 1) = 0;
	mcrRotYawMat(0, 2) = -sin(mcrRot.Yaw); mcrRotYawMat(1, 2) = 0; mcrRotYawMat(2, 2) = cos(mcrRot.Yaw);

	cml::matrix33d mclRotRollMat;
	mclRotRollMat(0, 0) = cos(mclRot.Roll); mclRotRollMat(1, 0) = -sin(mclRot.Roll); mclRotRollMat(2, 0) = 0;
	mclRotRollMat(0, 1) = sin(mclRot.Roll); mclRotRollMat(1, 1) = cos(mclRot.Roll); mclRotRollMat(2, 1) = 0;
	mclRotRollMat(0, 2) = 0; mclRotRollMat(1, 2) = 0; mclRotRollMat(2, 2) = 1;

	cml::matrix33d mclRotPitchMat;
	mclRotPitchMat(0, 0) = 1; mclRotPitchMat(1, 0) = 0; mclRotPitchMat(2, 0) = 0;
	mclRotPitchMat(0, 1) = 0; mclRotPitchMat(1, 1) = cos(mclRot.Pitch); mclRotPitchMat(2, 1) = -sin(mclRot.Pitch);
	mclRotPitchMat(0, 2) = 0; mclRotPitchMat(1, 2) = sin(mclRot.Pitch); mclRotPitchMat(2, 2) = cos(mclRot.Pitch);

	cml::matrix33d mclRotYawMat;
	mclRotYawMat(0, 0) = cos(mclRot.Yaw); mclRotYawMat(1, 0) = 0; mclRotYawMat(2, 0) = sin(mclRot.Yaw);
	mclRotYawMat(0, 1) = 0; mclRotYawMat(1, 1) = 1; mclRotYawMat(2, 1) = 0;
	mclRotYawMat(0, 2) = -sin(mclRot.Yaw); mclRotYawMat(1, 2) = 0; mclRotYawMat(2, 2) = cos(mclRot.Yaw);

	mcrRotMat = mcrRotYawMat * mcrRotPitchMat * mcrRotRollMat;
	mclRotMat = mclRotYawMat * mclRotPitchMat * mclRotRollMat;

	//temp = p1;
	p1 = MCR->GetComponentLocation();
	d1 = p1 - mid_;
	d1 = d1 * inv_;
	
	//temp = p2;
	p2 = MCL->GetComponentLocation();
	d2 = p2 - mid_;
	d2 = d2 * inv_;

	//temp = p3;
	p3 = HMD->GetComponentLocation();
	d3 = p3 - mid_;
	d3 = d3 * inv_;

	//temp = p4;
	p4 = TR1->GetComponentLocation();
	d4 = p4 - mid_;
	d4 = d4 * inv_;

	//temp = p5;
	p5 = TR2->GetComponentLocation();
	d5 = p5 - mid_;
	d5 = d5 * inv_;

	//temp = p6;
	p6 = TR3->GetComponentLocation();
	d6 = p6 - mid_;
	d6 = d6 * inv_;

	if (!CurrentVelocity.IsZero())
	{
		FVector NewLoc = GetActorLocation() + (CurrentVelocity * DeltaTime);
		SetActorLocation(NewLoc);
	}
	if (TelOn)
		ActLoc = GetActorLocation();

}

// Called to bind functionality to input
void AMyPawnVR::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("MCR_TriggerAction", EInputEvent::IE_Pressed, this, &AMyPawnVR::Input_MCR_TriggerAction_DOWN);
	PlayerInputComponent->BindAction("MCR_TriggerAction", EInputEvent::IE_Released, this, &AMyPawnVR::Input_MCR_TriggerAction_UP);
	PlayerInputComponent->BindAction("MCL_TriggerAction", EInputEvent::IE_Pressed, this, &AMyPawnVR::Input_MCL_TriggerAction_DOWN);
	PlayerInputComponent->BindAction("MCL_TriggerAction", EInputEvent::IE_Released, this, &AMyPawnVR::Input_MCL_TriggerAction_UP);
	PlayerInputComponent->BindAction("MCL_ShoulderAction", EInputEvent::IE_Pressed, this, &AMyPawnVR::Input_MCL_ShoulderAction_DOWN);
	PlayerInputComponent->BindAction("MCL_ShoulderAction", EInputEvent::IE_Released, this, &AMyPawnVR::Input_MCL_ShoulderAction_UP);
	PlayerInputComponent->BindAxis("MoveX", this, &AMyPawnVR::Move_XAxis);
	PlayerInputComponent->BindAxis("MoveY", this, &AMyPawnVR::Move_YAxis);
	PlayerInputComponent->BindAxis("MoveZ", this, &AMyPawnVR::Move_ZAxis);
}
void AMyPawnVR::Move_XAxis(float AxisValue)
{
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.f;
}

void AMyPawnVR::Move_YAxis(float AxisValue)
{
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.f;
}
void AMyPawnVR::Move_ZAxis(float AxisValue)
{
	CurrentVelocity.Z = FMath::Clamp(AxisValue, -1.0f, 1.0f) * 100.f;
}
void AMyPawnVR::Input_MCL_ShoulderAction_DOWN()
{
	TelOn = true;
}
void AMyPawnVR::Input_MCL_ShoulderAction_UP()
{
	TelOn = false;
}
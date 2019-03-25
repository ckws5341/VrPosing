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

	HMD = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("HeadMountDisplay"));
	HMD->SetupAttachment(CameraRootComponent);
	HMD->MotionSource = FXRMotionControllerBase::HMDSourceId;

	MCR = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_R"));
	MCR->SetupAttachment(CameraRootComponent);
	MCR->MotionSource = FXRMotionControllerBase::RightHandSourceId;

	MCL = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_L"));
	MCL->SetupAttachment(CameraRootComponent);
	MCL->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	
	TR1 = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Tracker1"));
	TR1->SetupAttachment(CameraRootComponent);
	//TR1->Hand_DEPRECATED = EControllerHand::Special_1;
	TR1->MotionSource;


	TR2 = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Tracker2"));
	TR2->SetupAttachment(CameraRootComponent);
	//TR2->Hand_DEPRECATED = EControllerHand::Special_2;
	TR2->MotionSource;

	TR3 = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Tracker3"));
	TR3->SetupAttachment(CameraRootComponent);
	//TR3->Hand_DEPRECATED = EControllerHand::Special_3;
	TR3->MotionSource;

}

// Called when the game starts or when spawned
void AMyPawnVR::BeginPlay()
{
	Super::BeginPlay();
	p1 = p2 = p3 = p4 = p5 = p6 = { 0.f, 1.f, 0.f };
	d1 = d2 = d3 = d4 = d5 = d6 = temp = { 0.f, 0.f, 0.f };

	for (auto &d : ms_ps)
	{
		d = FVector(0.f, 0.f, 0.f);
	}

	for (auto &d : ms_qs)
	{
		d = FQuat::Identity;
	}
}

// Called every frame
void AMyPawnVR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MCR->Activate(true);
	MCL->Activate(true);
	UWorld *MyWorld = GetWorld();


	ms_ps[AMyActor::MS_HEAD] = HMD->GetComponentLocation();
	ms_ps[AMyActor::MS_L_WRIST] = MCL->GetComponentLocation();
	ms_ps[AMyActor::MS_R_WRIST] = MCR->GetComponentLocation();
	ms_ps[AMyActor::MS_L_ANKLE] = TR1->GetComponentLocation();
	ms_ps[AMyActor::MS_R_ANKLE] = TR2->GetComponentLocation();
	ms_ps[AMyActor::MS_PELVIS] = TR3->GetComponentLocation();


	ms_qs[AMyActor::MS_HEAD] = HMD->GetComponentQuat();
	ms_qs[AMyActor::MS_L_WRIST] = MCL->GetComponentQuat();
	ms_qs[AMyActor::MS_R_WRIST] = MCR->GetComponentQuat();
	ms_qs[AMyActor::MS_L_ANKLE] = TR1->GetComponentQuat();
	ms_qs[AMyActor::MS_R_ANKLE] = TR2->GetComponentQuat();
	ms_qs[AMyActor::MS_PELVIS] = TR3->GetComponentQuat();



	temp, d1, d2, d3, d4, d5, d6 = { 0.f, 0.f, 0.f };
	//FVector mid_ = { -70.f, 180.f, 200.f };
	//FVector inv_ = { -1.f, -1.f, 1.f };
	
	FQuat q = MCR->GetComponentQuat();
	mcrRotMat = CalRotation(q);
	q = MCL->GetComponentQuat();
	mclRotMat = CalRotation(q);
	q = HMD->GetComponentQuat();
	hmdRotMat = CalRotation(q);
	
	q = TR1->GetComponentQuat();
	tr1RotMat = CalRotation(q);
	q = TR2->GetComponentQuat();
	tr2RotMat = CalRotation(q);
	q = TR3->GetComponentQuat();
	tr3RotMat = CalRotation(q);

	//temp = p1;
	d1 = p1 = MCR->GetComponentLocation();
	//d1 = p1 -mid_;
	//d1 = d1 * inv_;

	//temp = p2;
	d2 = p2 = MCL->GetComponentLocation();
	//d2 = p2 -mid_;
	//d2 = d2 * inv_;

	//temp = p3;
	d3 = p3 = HMD->GetComponentLocation();
	//d3 = p3 - mid_;
	//d3 = d3 * inv_;

	//temp = p4;
	d4 = p4 = TR1->GetComponentLocation();
	//d4 = p4 - mid_;
	//d4 = d4 * inv_;

	//temp = p5;
	d5 = p5 = TR2->GetComponentLocation();
	//d5 = p5 - mid_;
	//d5 = d5 * inv_;

	//temp = p6;
	d6 = p6 = TR3->GetComponentLocation();
	//d6 = p6 - mid_;
	//d6 = d6 * inv_;

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
	PlayerInputComponent->BindAction("ResetHumanT", EInputEvent::IE_Pressed, this, &AMyPawnVR::ResetHumanT);
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
	return;
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

cml::matrix33d AMyPawnVR::CalRotation(FQuat q)
{
	cml::matrix33d RotMat;
	cml::quaterniond cml_q;
	cml_q[cml::quaterniond::W] = q.W;
	cml_q[cml::quaterniond::X] = q.X;
	cml_q[cml::quaterniond::Y] = q.Y;
	cml_q[cml::quaterniond::Z] = q.Z;

	cml::matrix_rotation_quaternion(RotMat, cml_q);

	/*RotMat(0, 0) = 1 - 2 * q.Y*q.Y - 2 * q.Z*q.Z; RotMat(1, 0) = 2 * q.X*q.Y - 2 * q.W*q.Z; RotMat(2, 0) = 2 * q.X*q.Z + 2 * q.W*q.Y;
	RotMat(0, 1) = 2 * q.X*q.Y + 2 * q.W*q.Z; RotMat(1, 1) = 1 - 2 * q.X*q.X - 2 * q.Z*q.Z; RotMat(2, 1) = 2 * q.Y*q.Z - 2 * q.W*q.X;
	RotMat(0, 2) = 2 * q.X*q.Z - 2 * q.W*q.Y; RotMat(1, 2) = 2 * q.Y*q.Z + 2 * q.W*q.X; RotMat(2, 2) = 1 - 2 * q.X*q.X - 2 * q.Y*q.Y;*/

	return RotMat;
}

void AMyPawnVR::ResetHumanT()
{

	for (TActorIterator<AMyActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		if (ActorItr->GetName().Compare(TEXT("James")) == 0)
		{
			ActorItr->ResetHumanT();
		}
	}
}

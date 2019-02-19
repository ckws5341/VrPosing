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
	CameraRootComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));

	VRCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCameraComponent->SetupAttachment(CameraRootComponent);

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
	p1, p2, p3, p4, p5, p6 = { 0.f, 0.f, 0.f };
}

// Called every frame
void AMyPawnVR::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);
	MCR->Activate(true);
	MCL->Activate(true);
	UWorld *MyWorld = GetWorld();
	temp, d1, d2, d3, d4, d5, d6 = { 0.f, 0.f, 0.f };
	
	FVector inv_ = { 1.f, -1.f, 1.f };

	temp = p1;
	p1 = MCR->GetComponentLocation();
	d1 = p1 - temp;
	d1 = d1 * inv_;
	
	temp = p2;
	p2 = MCL->GetComponentLocation();
	d2 = p2 - temp;
	d2 = d2 * inv_;

	temp = p3;
	p3 = HMD->GetComponentLocation();
	d3 = p3 - temp;
	d3 = d3 * inv_;

	temp = p4;
	p4 = TR1->GetComponentLocation();
	d4 = p4 - temp;
	d4 = d4 * inv_;

	temp = p5;
	p5 = TR2->GetComponentLocation();
	d5 = p5 - temp;
	d5 = d5 * inv_;

	temp = p6;
	p6 = TR3->GetComponentLocation();
	d6 = p6 - temp;
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
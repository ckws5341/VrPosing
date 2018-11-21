// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawnVR.h"


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

	MCR = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_R"));
	MCR->SetupAttachment(CameraRootComponent);
	MCR->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	MCR->Hand_DEPRECATED = EControllerHand::Right;

	MCL = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController_L"));
	MCL->SetupAttachment(CameraRootComponent);
	MCL->MotionSource = FXRMotionControllerBase::LeftHandSourceId;
	MCL->Hand_DEPRECATED = EControllerHand::Left;

}

// Called when the game starts or when spawned
void AMyPawnVR::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPawnVR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MCR->Activate(true);
	MCL->Activate(true);
}

// Called to bind functionality to input
void AMyPawnVR::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("MCR_TriggerAction", EInputEvent::IE_Pressed, this, &AMyPawnVR::Input_MCR_TriggerAction_DOWN);
	PlayerInputComponent->BindAction("MCR_TriggerAction", EInputEvent::IE_Released, this, &AMyPawnVR::Input_MCR_TriggerAction_UP);
	PlayerInputComponent->BindAction("MCL_TriggerAction", EInputEvent::IE_Pressed, this, &AMyPawnVR::Input_MCL_TriggerAction_DOWN);
	PlayerInputComponent->BindAction("MCL_TriggerAction", EInputEvent::IE_Released, this, &AMyPawnVR::Input_MCL_TriggerAction_UP);

}


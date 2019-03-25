// Fill out your copyright notice in the Description page of Project Settings.

#include "UserHandActorVR.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "UObject.h"
#include "MyPawnVR.h"


// Sets default values
AUserHandActorVR::AUserHandActorVR()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Handmesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handmesh"));
	Handmesh->SetupAttachment(RootComponent);

	const TCHAR * path = _T("/Game/hand/hand_v1");
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshRef(path);
	
	if(MeshRef.Object)
	{
		Handmesh->SetStaticMesh(MeshRef.Object);
	}
}

// Called when the game starts or when spawned
void AUserHandActorVR::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUserHandActorVR::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UWorld *MyWorld = GetWorld();
	bool handgrip = false;
	if (MyWorld)
	{
		AMyPawnVR* pVR = Cast<AMyPawnVR>(MyWorld->GetFirstPlayerController()->GetPawn());
		FVector p = { 1.f, 0.f, 0.f };
		if (pVR)
		{
			if (pVR->ms_ps[AMyActor::MS_R_WRIST] != p)
			{
				p = pVR->ms_ps[AMyActor::MS_R_WRIST];
				
			}
		}
	}
}


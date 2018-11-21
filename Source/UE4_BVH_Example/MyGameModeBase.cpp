// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameModeBase.h"

AActor * AMyGameModeBase::ChoosePlayerStart_Implementation(AController * Player)
{
	auto ps = GetComponentsByTag(TSubclassOf<UActorComponent>(), TEXT("MyPS"));
	if ( ps.Num()==0 ) return Super::ChoosePlayerStart_Implementation(Player);
	 UE_LOG(LogTemp, Warning, TEXT("ps.Num() = %d"), ps.Num()); // 디버그 용 출력

	return (AActor*)ps[0];

	return nullptr;
}

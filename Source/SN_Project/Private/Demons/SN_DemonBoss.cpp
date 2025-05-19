// Fill out your copyright notice in the Description page of Project Settings.


#include "Demons/SN_DemonBoss.h"

#include "Demons/SN_DemonSubsystem.h"
#include "Demons/SN_SpawnDemon.h"
#include "Kismet/GameplayStatics.h"
#include "Library/SN_Library.h"

void ASN_DemonBoss::ActiveDemon()
{
	
	if (!IsValid(DestinyFloor))
	{
		return;
	}
	
	AActor* ActiveActor = USN_Library::GetActiveActorFloor(this);

	if (!ActiveActor)
	{
		return;
	}

	if (ActiveActor == DestinyFloor)
	{		
		DemonEndActionsDelegate.Broadcast();
		DeathDemonDelegate.Broadcast(0);

		bIsExit = true;
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASN_DemonBoss::DestroyDemon, 2.0f, false);
		//SetActorHiddenInGame(true);
	}

	if (DemonSubsystem && ActiveActor != OriginFloor)
	{
		IIMovement* ActiveFloor = Cast<IIMovement>(ActiveActor);
		DemonSubsystem->SpawnDemonsInFloor(ActiveFloor, true);
	}
	
//	Movement(DestinyFloor, aCharacters, false);
	
}

void ASN_DemonBoss::BeginPlay()
{

	Super::BeginPlay();

	DemonSubsystem = UWorld::GetSubsystem<USN_DemonSubsystem>(GetWorld());
	SearchSpawnActorToExit();
	
}

void ASN_DemonBoss::SearchSpawnActorToExit()
{

	AActor* ActiveActor = USN_Library::GetActiveActorFloor(this);
	OriginFloor = ActiveActor;
	
	TArray<AActor*> TempActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASN_SpawnDemon::StaticClass(), TempActors);

	float FarDistance = 0.f;
	AActor* TmpSpawner = nullptr;
	for (AActor* Actor : TempActors)
	{

		ISN_Spawner* Spawner = Cast<ISN_Spawner>(Actor);

		if (!Spawner || (!Spawner->ISEnabled()))
		{
			continue;
		}

		float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());

		if (Distance > FarDistance)
		{
			FarDistance = Distance;
			TmpSpawner = Actor;
		}
		
	}

	if (AActor* SpawnerFloor = USN_Library::GetActiveActorFloor(TmpSpawner))
	{
		DestinyFloor = SpawnerFloor;
	}

	if (!IsValid(DestinyFloor))
	{		
		DestinyFloor = OriginFloor;
	}
	
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SN_Door.h"

#include "Actors/IMovement.h"
#include "Demons/DemonInterface.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASN_Door::ASN_Door()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DoorRootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = DoorRootComponent;

	SMDoor = CreateDefaultSubobject<UStaticMeshComponent>("SMDoor");
	SMDoor->SetupAttachment(DoorRootComponent);

	bDoorClose = true;
	bDoorLocked = false;
}

// Called when the game starts or when spawned
void ASN_Door::BeginPlay()
{
	Super::BeginPlay();
}

void ASN_Door::DestroyDoor()
{
	for (AActor* Actor : aFloorsToOpen)
	{
		if (IIMovement* Floor = Cast<IIMovement>(Actor))
		{
			Floor->SetClose(false);
		}
	}

	for (AStaticMeshActor* Actor : aRoofs)
	{
		Actor->Destroy();
	}

	for (AActor* Actor : aDemonsToActive)
	{
		if (IDemonInterface* Demon = Cast<IDemonInterface>(Actor))
		{
			Demon->ActiveDemon();
		}
	}

	bDoorClose = false;

	if (BreakDoorSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, BreakDoorSoundCue, 1.0f, 1.0f, 0.0f);
	}
	
	PlayOpenDoorParticles();

	SMDoor->SetHiddenInGame(true);
	SetLifeSpan(2.f);

//	Destroy();
}

void ASN_Door::PlaySoundHitDoor()
{
	if (OpenDoorSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, OpenDoorSoundCue, 1.0f, 1.0f, 0.0f);
	}
}

void ASN_Door::PlayFailOpenDoorParticles()
{
	if (FailOpenDoorParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FailOpenDoorParticles, GetActorLocation(), GetActorRotation(), true);
	}
}

void ASN_Door::PlayOpenDoorParticles()
{
	if (OpenDoorParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OpenDoorParticles, GetActorLocation(), GetActorRotation(), true);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SN_Floor.h"
#include "Actors/SN_Door.h"
#include "Actors/SN_Objective.h"
#include "Camera/CameraComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Demons/SN_DemonSubsystem.h"
#include "GameFramework/Character.h"
#include "Library/SN_Library.h"

// Sets default values
ASN_Floor::ASN_Floor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	FloorRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FloorRootComponent"));
	RootComponent = FloorRootComponent;

	SMFloor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMFloor"));
	SMFloor->SetupAttachment(RootComponent);

	DetectPlayerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectPlayerCollision"));
	DetectPlayerCollision->SetupAttachment(RootComponent);

	SMViewActiveFloorLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMViewActiveFloorLeft"));
	SMViewActiveFloorLeft->SetupAttachment(RootComponent);

	SMViewActiveFloorRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMViewActiveFloorRight"));
	SMViewActiveFloorRight->SetupAttachment(RootComponent);
	
	DetectDemonCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectDemonCollision"));
	DetectDemonCollision->SetupAttachment(RootComponent);

	DetectFloorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectFloorCollision"));
	DetectFloorCollision->SetupAttachment(RootComponent);

	OpenDoorLocation = CreateDefaultSubobject<UBillboardComponent>(TEXT("OpenDoorLocation"));
	OpenDoorLocation->SetupAttachment(RootComponent);

	FloorCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FloorCamera"));
	FloorCamera->SetupAttachment(RootComponent);
	
	bActiveMovement = false;
	bClose = false;
	bCanSearch = false;
	bSpawnDemonsInFloor = true;
}

// Called when the game starts or when spawned
void ASN_Floor::BeginPlay()
{
	Super::BeginPlay();
}

void ASN_Floor::SetState(bool bNewState)
{
	BP_SetState(bNewState);
}

void ASN_Floor::ResetMovement()
{
	bActiveMovement = false;
}

void ASN_Floor::OpenDoor()
{
	
	DoorRef->DestroyDoor();

	if (!DoorRef->GetFloorsToOpen().IsEmpty())
	{
		if (USN_DemonSubsystem* DemonSubsystem = UWorld::GetSubsystem<USN_DemonSubsystem>(GetWorld()))
		{
			DemonSubsystem->SpawnDemonsInFloors(DoorRef->GetFloorsToOpen());
		}
	}

}

FVector ASN_Floor::GetDoorLocation()
{

	if (DoorRef)
	{
		return DoorRef->GetActorLocation();
	}

	return FVector::ZeroVector;
	
}

bool ASN_Floor::ExistDoor()
{
	if (!DoorRef || (!DoorRef->IsDoorClose()))
	{
		USN_Library::ShowMessageToPlayer(GetWorld(), "NO EXISTE NINGUNA PUERTA!!!");		
		return false;
	}

	if (DoorRef->IsDoorLocked())
	{
		USN_Library::ShowMessageToPlayer(GetWorld(), "LA PUERTA ESTA BLOQUEDA!!!");		
		return false;
	}

	return true;
}

TArray<AActor*> ASN_Floor::GetOverlappingFloors() const
{
	TArray<AActor*> aReturn;

	DetectFloorCollision->GetOverlappingActors(aReturn, ASN_Floor::StaticClass());

	return aReturn;
}

TArray<AActor*> ASN_Floor::GetOverlappingCharacters() const
{
	TArray<AActor*> aReturn;

	DetectPlayerCollision->GetOverlappingActors(aReturn, ACharacter::StaticClass());

	return aReturn;
}

TArray<AActor*> ASN_Floor::GetOverlappingDemons() const
{
	TArray<AActor*> aReturn;

	DetectDemonCollision->GetOverlappingActors(aReturn, nullptr);

	return aReturn;
}


void ASN_Floor::InitMovement()
{
	BP_InitMovement();
}

void ASN_Floor::EnableDisableMovement(bool bEnable)
{
	SMViewActiveFloorLeft->SetHiddenInGame(!bEnable);
	SMViewActiveFloorRight->SetHiddenInGame(!bEnable);
	bActiveMovement = bEnable;
}

void ASN_Floor::ShowActiveFloorsToMovement()
{
	TArray<AActor*> OverlappingFloors = GetOverlappingFloors();

	for (AActor* ActorFloor : OverlappingFloors)
	{
		if (ActorFloor == this)
		{
			continue;
		}

		IIMovement* Floor = Cast<IIMovement>(ActorFloor);

		if (!Floor || (Floor->IsClose()))
		{
			continue;
		}

		Floor->EnableDisableMovement(true);
	}
}

void ASN_Floor::GetFloorsToAttack(TArray<IIMovement*>& aFloorsReturn, IIMovement* const ActualFloor, int Distance)
{
	TArray<AActor*> OverlappingFloors = ActualFloor->GetOverlappingFloors();

	TArray<IIMovement*> aFloorsToRevise;

	for (AActor* ActorFloor : OverlappingFloors)
	{
		IIMovement* Floor = Cast<IIMovement>(ActorFloor);

		if (!Floor)
		{
			continue;
		}

		aFloorsReturn.Add(Floor);
		aFloorsToRevise.Add(Floor);
	}

	--Distance;

	if (Distance == 0)
	{
		return;
	}

	for (IIMovement* Floor : aFloorsToRevise)
	{
		Floor->GetFloorsToAttack(aFloorsReturn, Floor, Distance);
	}
}

bool ASN_Floor::CanSearch()
{
	return bCanSearch;
}

FVector ASN_Floor::GetRandomLocationInFloor()
{
	FVector BoxOrigin = DetectPlayerCollision->GetComponentLocation();
	FVector BoxExtent = DetectPlayerCollision->GetScaledBoxExtent();

	float X = BoxExtent.X - 80;
	float Y = BoxExtent.Y - 80;
	
	float RandX = FMath::RandRange(-X, X);
	float RandY = FMath::RandRange(-Y, Y);

	return BoxOrigin + FVector(RandX, RandY, BoxExtent.Z);
}

FVector ASN_Floor::GetActorLocation()
{
	return Super::GetActorLocation();
}

FRotator ASN_Floor::GetActorRotation()
{
	return Super::GetActorRotation();
}

bool ASN_Floor::CanSpawnDemonsInFloor()
{
	return bSpawnDemonsInFloor;
}

AActor* ASN_Floor::GetObjectiveActor()
{
	TArray<AActor*> aReturn;

	DetectPlayerCollision->GetOverlappingActors(aReturn, ASN_Objective::StaticClass());

	if (aReturn.IsEmpty())
	{
		return nullptr;
	}

	return aReturn[0];
}

FVector ASN_Floor::GetLocationOpenDoor()
{

	if (OpenDoorLocation)
	{
		return OpenDoorLocation->GetComponentLocation();
	}
	
	return FVector::ZeroVector;

}

void ASN_Floor::PlayVFXHitDoor()
{

	if (IsValid(DoorRef))
	{
		DoorRef->PlaySoundHitDoor();
		DoorRef->PlayFailOpenDoorParticles();
	}
	
}

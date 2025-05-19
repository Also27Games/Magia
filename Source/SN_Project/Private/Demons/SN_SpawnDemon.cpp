// Fill out your copyright notice in the Description page of Project Settings.


#include "Demons/SN_SpawnDemon.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values
ASN_SpawnDemon::ASN_SpawnDemon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SpawnRootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SpawnRootComponent);
	
	SMSpawnFloor1 = CreateDefaultSubobject<UStaticMeshComponent>("SMSpawnFloor1");
	SMSpawnFloor1->SetupAttachment(RootComponent);

	SMSpawnFloor2 = CreateDefaultSubobject<UStaticMeshComponent>("SMSpawnFloor2");
	SMSpawnFloor2->SetupAttachment(RootComponent);

	SMSpawnFloor3 = CreateDefaultSubobject<UStaticMeshComponent>("SMSpawnFloor3");
	SMSpawnFloor3->SetupAttachment(RootComponent);

	SMSpawnFloor4 = CreateDefaultSubobject<UStaticMeshComponent>("SMSpawnFloor4");
	SMSpawnFloor4->SetupAttachment(RootComponent);
	
	SMPortal = CreateDefaultSubobject<UStaticMeshComponent>("SMPortal");
	SMPortal->SetupAttachment(SpawnRootComponent);

	LightComponent = CreateDefaultSubobject<UPointLightComponent>("LightComponent");
	LightComponent->SetupAttachment(SpawnRootComponent);

	SpawnBoxComponent = CreateDefaultSubobject<UBoxComponent>("SpawnLocationComponent");
	SpawnBoxComponent->SetupAttachment(RootComponent);

	SpawnCamera = CreateDefaultSubobject<UCameraComponent>("SpawnCamera");
	SpawnCamera->SetupAttachment(RootComponent);
	
	ArrowSpawn = CreateDefaultSubobject<UArrowComponent>("ArrowSpawn");
	ArrowSpawn->SetupAttachment(SpawnRootComponent);

	bEnabled = true;
	
}

// Called when the game starts or when spawned
void ASN_SpawnDemon::BeginPlay()
{
	Super::BeginPlay();

	if (!bEnabled)
	{
		SMPortal->SetHiddenInGame(true);
		LightComponent->SetHiddenInGame(true);
	}
	
}

bool ASN_SpawnDemon::ISEnabled() const
{
	return bEnabled;
}

void ASN_SpawnDemon::Active()
{
	bEnabled = true;
	SMPortal->SetHiddenInGame(false);
	LightComponent->SetHiddenInGame(false);
}

FVector ASN_SpawnDemon::GetActorLocation() const
{
	return Super::GetActorLocation();	
}

FRotator ASN_SpawnDemon::GetActorRotation() const
{
	return Super::GetActorRotation();
}

FVector ASN_SpawnDemon::GetSpawnLocation() const
{

	FVector BoxOrigin = SpawnBoxComponent->GetComponentLocation();
	FVector BoxExtent = SpawnBoxComponent->GetScaledBoxExtent();

	float X = BoxExtent.X - 0;
	float Y = BoxExtent.Y - 0;

	float RandX = FMath::RandRange(-X, X);
	float RandY = FMath::RandRange(-Y, Y);

	return BoxOrigin + FVector(RandX, RandY, 0);
	
}

UCameraComponent* ASN_SpawnDemon::GetSpawnCamera() const
{
	return SpawnCamera;
}

void ASN_SpawnDemon::ActiveNormalSpawn()
{
	BP_ActiveNormalSpawn();
}

void ASN_SpawnDemon::ActiveSuperSpawn()
{
	BP_ActiveSuperSpawn();
}


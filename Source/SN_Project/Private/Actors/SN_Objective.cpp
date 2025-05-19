// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SN_Objective.h"

// Sets default values
ASN_Objective::ASN_Objective()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ObjectiveRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ObjectiveRootComponent"));
	RootComponent = ObjectiveRootComponent;

	SMObjective = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMObjective"));
	SMObjective->SetupAttachment(RootComponent);

	bActive = true;
	bExit = false;
	bRealObjective = false;

	ExpValue = 5;
}

// Called when the game starts or when spawned
void ASN_Objective::BeginPlay()
{
	Super::BeginPlay();

	Enable(bActive);
}


bool ASN_Objective::Take(AActor* Character)
{

	bool Success = false;

	if (bRealObjective || bExit)
	{
		ObjectiveTakeDelegate.Broadcast(ExpValue);
		Success = true;
	}
	
	if (!bExit)
	{
		Enable(false);
	}
	
	return Success;
	
}

void ASN_Objective::Enable(bool bEnable)
{
	bActive = bEnable;
	SetActorHiddenInGame(!bEnable);
}

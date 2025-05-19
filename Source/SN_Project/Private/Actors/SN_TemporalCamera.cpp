// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SN_TemporalCamera.h"

#include "Camera/CameraComponent.h"


// Sets default values
ASN_TemporalCamera::ASN_TemporalCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	TmpCameraRootComponent = CreateDefaultSubobject<USceneComponent>("TmpCameraRootComponent");
	RootComponent = TmpCameraRootComponent;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ASN_TemporalCamera::BeginPlay()
{
	Super::BeginPlay();	
}



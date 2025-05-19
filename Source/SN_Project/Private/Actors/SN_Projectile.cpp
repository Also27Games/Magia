// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SN_Projectile.h"

// Sets default values
ASN_Projectile::ASN_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

}

// Called when the game starts or when spawned
void ASN_Projectile::BeginPlay()
{
	Super::BeginPlay();	
}

void ASN_Projectile::MoveToTarget(const FVector TargetLocation)
{
	BP_MoveToTarget(TargetLocation);
}



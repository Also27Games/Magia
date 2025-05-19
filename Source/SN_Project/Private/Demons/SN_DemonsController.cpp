// Fill out your copyright notice in the Description page of Project Settings.


#include "Demons/SN_DemonsController.h"
#include "Navigation/CrowdFollowingComponent.h"

/*ASN_DemonsController::ASN_DemonsController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	
}*/

ASN_DemonsController::ASN_DemonsController()
{
}

void ASN_DemonsController::BeginPlay()
{
	Super::BeginPlay();

	/*if (UCrowdFollowingComponent* CrowdFollowingComp = FindComponentByClass<UCrowdFollowingComponent>())
	{
		CrowdFollowingComp->SetCrowdSeparation(true);
		CrowdFollowingComp->SetCrowdSeparationWeight(10.f);
		CrowdFollowingComp->SetCrowdAvoidanceRangeMultiplier(1.1f);
	}*/

	
	
}

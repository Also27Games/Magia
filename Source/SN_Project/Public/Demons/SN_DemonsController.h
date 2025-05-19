// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SN_DemonsController.generated.h"

/**
 * 
 */
UCLASS()
class SN_PROJECT_API ASN_DemonsController : public AAIController
{
	GENERATED_BODY()

public:
	//ASN_DemonsController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	ASN_DemonsController();

protected:
	void BeginPlay() override;
};

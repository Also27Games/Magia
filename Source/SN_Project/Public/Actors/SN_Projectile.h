// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SN_Projectile.generated.h"

UCLASS()
class SN_PROJECT_API ASN_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASN_Projectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override; 

public:

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(const FVector TargetLocation);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_MoveToTarget(FVector TargetLocation);

};

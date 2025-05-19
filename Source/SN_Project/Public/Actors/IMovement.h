// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "IMovement.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UIMovement : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SN_PROJECT_API IIMovement
{
	GENERATED_BODY()

public:
	virtual bool IsActiveMovement() = 0;

	virtual void EnableDisableMovement(bool bEnable) = 0;

	virtual void InitMovement() = 0;

	virtual bool IsClose() = 0;

	virtual void SetClose(bool bClose) = 0;

	virtual TArray<AActor*> GetOverlappingFloors() const = 0;

	virtual TArray<AActor*> GetOverlappingCharacters() const = 0;

	virtual TArray<AActor*> GetOverlappingDemons() const = 0;

	virtual void ShowActiveFloorsToMovement() = 0;

	virtual void OpenDoor() = 0;

	virtual FVector GetDoorLocation() = 0;

	virtual bool ExistDoor() = 0;

	virtual FVector GetLocationOpenDoor() = 0;

	virtual void GetFloorsToAttack(TArray<IIMovement*>& aFloorsReturn, IIMovement* const ActualFloor, int Distance) = 0;

	virtual bool CanSearch() = 0;

	virtual FVector GetRandomLocationInFloor() = 0;

	virtual FVector GetActorLocation() = 0;

	virtual FRotator GetActorRotation() = 0;

	virtual bool CanSpawnDemonsInFloor() = 0;

	virtual AActor* GetObjectiveActor() = 0;

	virtual void PlayVFXHitDoor() = 0;
	
};

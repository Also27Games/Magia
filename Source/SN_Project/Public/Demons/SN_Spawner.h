// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/SN_TemporalCamera.h"
#include "UObject/Interface.h"
#include "SN_Spawner.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USN_Spawner : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SN_PROJECT_API ISN_Spawner
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool ISEnabled() const = 0;

	virtual void Active() = 0;

	virtual FVector GetActorLocation() const = 0;

	virtual FRotator GetActorRotation() const  = 0;

	/*
	 * Devuele un punto aleatorio de Spawn dentro del BoxCollision de spawn
	 */
	virtual FVector GetSpawnLocation() const = 0;

	virtual UCameraComponent* GetSpawnCamera() const = 0;

	virtual void ActiveNormalSpawn() = 0;

	virtual void ActiveSuperSpawn() = 0;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SN_PROJECT_API IHealthInterface
{
	GENERATED_BODY()


	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void RecieveDamage(int Damage) = 0;

	virtual bool IsDeath() const = 0;

	virtual int GetHealth() const = 0;

	/*
	 * Devuele el nivel mínimo del arma para poder recibir daño
	 */
	virtual int GetMinimunForceToDamage() const = 0;

	virtual void AddHealth(int NewHealth) = 0;

	virtual void SetHealth(int NewHealth) = 0;

	virtual bool IsInvisible() const  = 0;

	virtual void SetInvisible(bool bNewInvisible) = 0;

	virtual FVector GetLocation() const = 0;

	virtual void ClearCountEnemiesAttackMe() = 0;

	virtual int GetCountEnemiesAttackMe() const = 0;

	virtual void AddCountEnemiesAttackMe() = 0;

	virtual FVector GetFreePostionToAttack() = 0;
	
	
};

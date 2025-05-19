// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SN_HealtComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateHealthDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SN_PROJECT_API USN_HealtComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	
	int CurrentHealth;

	

	bool bIsDead;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	int Inithealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	int MinimunForceToDamage;
	
public:

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDeathDelegate DeathDelegate;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnUpdateHealthDelegate UpdateHealthDelegate;
	
public:	
	// Sets default values for this component's properties
	USN_HealtComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintCallable, Category="Health")
	void RecieveDamage(int Damage);	

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Health")
	bool IsDeath() const;	

	UFUNCTION(BlueprintCallable, Category="Health")
	int GetCurrentHealth() const;

	virtual int GetMinimunForceToDamage();

	void AddHealth(int NewHealth);

	void SetHealth(int NewHealth);
	
};

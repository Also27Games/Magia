// Fill out your copyright notice in the Description page of Project Settings.


#include "SN_Components/SN_HealtComponent.h"

// Sets default values for this component's properties
UPROPERTY(Blueprintable)
USN_HealtComponent::USN_HealtComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	Inithealth = 3;
	MinimunForceToDamage = 1;
		
}


// Called when the game starts
void USN_HealtComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = Inithealth;
	
}

void USN_HealtComponent::RecieveDamage(int Damage)
{

	CurrentHealth = CurrentHealth - Damage;

	UpdateHealthDelegate.Broadcast();
	
	if (CurrentHealth <= 0)
	{
		bIsDead = true;
		DeathDelegate.Broadcast();		
	}
	
}

bool USN_HealtComponent::IsDeath() const
{
	return bIsDead;
}

int USN_HealtComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

int USN_HealtComponent::GetMinimunForceToDamage()
{
	return MinimunForceToDamage;
}

void USN_HealtComponent::AddHealth(int NewHealth)
{
	CurrentHealth = CurrentHealth + NewHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0, Inithealth);
	UpdateHealthDelegate.Broadcast();
}

void USN_HealtComponent::SetHealth(int NewHealth)
{
	CurrentHealth = NewHealth;

	if (CurrentHealth <= 0)
	{
		bIsDead = true;
	}
	
}

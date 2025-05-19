// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SN_CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SN_PROJECT_API USN_CharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	void PlayLightMontageLeft();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayLightMontageLeft();

	void PlayLightMontageRight();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayLightMontageRight();

	void PlayHeavyMontageLeft();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayHeavyMontageLeft();

	void PlayHeavyMontageRight();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayHeavyMontageRight();

	void PlayMagicMontageLeft();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayMagicMontageLeft();

	void PlayMagicMontageRight();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlayMagicMontageRight();
	
};

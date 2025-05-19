// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SN_DemonAnimInstance.generated.h"

class UAnimMontage;

/**
 * 
 */
UCLASS()
class SN_PROJECT_API USN_DemonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	public:

	void PlayMontage(UAnimMontage* Montage);
	
};

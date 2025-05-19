// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SN_WCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SN_PROJECT_API USN_WCharacter : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateHealth(int NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateExp(int NewExp);
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharactersGameSave.generated.h"

/**
 *  Estuctura para Save Game de los Characters
 */
USTRUCT(BlueprintType)
struct SN_PROJECT_API FCharactersGameSave : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int CurrentHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int CurrentExp = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int LeftItemId = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int RightItemId = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int BodyItemId = -1;
	
};

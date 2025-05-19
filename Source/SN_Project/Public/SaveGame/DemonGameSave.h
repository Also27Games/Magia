// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DemonGameSave.generated.h"

class ASN_AbstractDemon;
/**
 * Estructura para el Save Game de los Demons
 */
USTRUCT(BlueprintType)
struct SN_PROJECT_API FDemonGameSave : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<ASN_AbstractDemon> DemonClass;
};

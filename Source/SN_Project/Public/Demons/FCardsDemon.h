// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FCardsDemon.generated.h"

class ASN_AbstractDemon;

/**
 *  Estructura de cartas de spawn de demonios
 */
USTRUCT(BlueprintType)
struct SN_PROJECT_API FCardsDemon : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bDoubleSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bDoubleActivation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<ASN_AbstractDemon> double_activation_type;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<ASN_AbstractDemon> level_1_type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int level_1_num;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<ASN_AbstractDemon> level_2_type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int level_2_num;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<ASN_AbstractDemon> level_3_type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int level_3_num;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<ASN_AbstractDemon> level_4_type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int level_4_num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bEnable;
	
};

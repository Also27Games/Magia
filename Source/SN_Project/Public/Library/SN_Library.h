// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SN_Library.generated.h"

/**
 *  Libreria General del proyecto
 */

class ASN_Floor;

UCLASS()
class SN_PROJECT_API USN_Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/*
	 * Devuelve el Floor donde se encuentra el Actor pasado por parametros
	 */
	UFUNCTION(BlueprintPure,  Category = "SN_Libary")
	static ASN_Floor* GetActiveFloor(const AActor* Target);

	/*
	 * Devuelve el Floor donde se encuentra el Actor pasado por parametros
	 */
	UFUNCTION(BlueprintPure,  Category = "SN_Libary")
	static AActor* GetActiveActorFloor(const AActor* Target);

	/*
	 * Devuelve un numero aleatorio de tirada (Entre 1 y 6)
	 */
	UFUNCTION(BlueprintPure,  Category = "SN_Libary")
	static int GetRandomDiceNumber();

	UFUNCTION(BlueprintCallable,  Category = "SN_Libary")
	static void ShowMessageToPlayer(const UObject* WorldContext, const FString& Message);
	
};

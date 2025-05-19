// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SN_Objective.generated.h"

class UStaticMeshComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveTakeDelegate, int32, ExpValue);

UCLASS()
class SN_PROJECT_API ASN_Objective : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* ObjectiveRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SMObjective;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	bool bActive;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	bool bExit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
	int ExpValue;

	bool bRealObjective;
	
public:
	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnObjectiveTakeDelegate ObjectiveTakeDelegate;

public:
	// Sets default values for this actor's properties
	ASN_Objective();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	/*
	 * Cuando un character coge el objectivo
	 */
	bool Take(AActor* Character);

	virtual void Enable(bool bEnable);

	/*
	 * Setea si el Objetivo es real o uno de mentira para hacer que el jugador busque más
	 */
	virtual void SetRealObjective(bool bNewRealObjective) {bRealObjective = bNewRealObjective;};

	/*
	 * Devuelve si el Objetivo es real o uno de mentira para hacer que el jugador busque más
	 */
	virtual bool IsRealObjective() { return bRealObjective; };
	
	void SetActive(bool bNewActive) { bActive = bNewActive; }

	bool IsActive() const { return bActive; }

	/*
	 * Indica si el objetivo es de salida del nivel o no
	 */
	bool IsExit() const { return bExit; }

	/*
	 * Indica cuanta experiencia gana el character que recoje el objetivo
	 */
	int GetExpValue() const { return ExpValue; }

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Demons/SN_AbstractDemon.h"
#include "SN_DemonBoss.generated.h"

class USN_DemonSubsystem;

/**
 *  Clase hija de SN_AbstractDemon con un comportamiento especial,
 *  ya que no ataca, solo busca una huida mientras spawnea mas demonios.
 */
UCLASS()
class SN_PROJECT_API ASN_DemonBoss : public ASN_AbstractDemon
{
	GENERATED_BODY()

private:

	AActor* OriginFloor;
	AActor* DestinyFloor;

	USN_DemonSubsystem* DemonSubsystem;
	
public:
	virtual void ActiveDemon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SearchSpawnActorToExit();

};

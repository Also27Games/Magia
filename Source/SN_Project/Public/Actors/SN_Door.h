// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SN_Door.generated.h"

class IDemonInterface;
class ASN_Floor;
class UStaticMeshComponent;
class USceneComponent;
class AStaticMeshActor;
class UParticleSystem;

UCLASS()
class SN_PROJECT_API ASN_Door : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor")
	USceneComponent* DoorRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor")
	UStaticMeshComponent* SMDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	bool bDoorClose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	bool bDoorLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	TArray<AActor*> aFloorsToOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	TArray<AStaticMeshActor*> aRoofs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Door")
	TArray<AActor*> aDemonsToActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds")
	USoundBase* OpenDoorSoundCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds")
	USoundBase* BreakDoorSoundCue;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* FailOpenDoorParticles;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* OpenDoorParticles;
	
public:
	// Sets default values for this actor's properties
	ASN_Door();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category="Door")
	void DestroyDoor();

	/*
	 * Lanza el timeline para la animación de la puerta
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void BP_OpenDoor();

	bool IsDoorClose() const
	{
		return bDoorClose;
	}

	bool IsDoorLocked() const { return bDoorLocked; }

	/*
	 * Indica que casillas se activan al abrir la puerta
	 */
	TArray<AActor*> GetFloorsToOpen() { return aFloorsToOpen; };

	void PlaySoundHitDoor();

	void PlayFailOpenDoorParticles();

	void PlayOpenDoorParticles();
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SN_Spawner.h"
#include "GameFramework/Actor.h"
#include "SN_SpawnDemon.generated.h"

class UBoxComponent;
class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;
class UArrowComponent;
class ASN_TemporalCamera;
class UCameraComponent;

UCLASS()
class SN_PROJECT_API ASN_SpawnDemon : public AActor, public ISN_Spawner
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USceneComponent* SpawnRootComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* SMSpawnFloor1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* SMSpawnFloor2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* SMSpawnFloor3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* SMSpawnFloor4;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UBoxComponent* SpawnBoxComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* SMPortal;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UPointLightComponent* LightComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UArrowComponent* ArrowSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UCameraComponent* SpawnCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	bool bEnabled;
	
public:	
	// Sets default values for this actor's properties
	ASN_SpawnDemon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual bool ISEnabled() const override;

	virtual void Active() override;

	virtual FVector GetActorLocation() const override;

	virtual FRotator GetActorRotation() const override;

	virtual FVector GetSpawnLocation() const override;

	virtual UCameraComponent* GetSpawnCamera() const override;

	virtual void ActiveNormalSpawn();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ActiveNormalSpawn();

	virtual void ActiveSuperSpawn();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ActiveSuperSpawn();
	
};

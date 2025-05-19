// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FCardsDemon.h"
#include "Subsystems/WorldSubsystem.h"
#include "Demons/SN_Spawner.h"
#include "Actors/IMovement.h"
#include "SaveGame/DemonGameSave.h"
#include "SN_DemonSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDemonDeathDelegate, int, ExpValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDemonsTurnEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllDemonsEndActionDelegate);

USTRUCT(BlueprintType)
struct FSpawnStruct
{
	GENERATED_BODY()

	int NumDemonsSpawned;
	TSubclassOf<ASN_AbstractDemon> DemonClass;
	
};

/**
 *  Subsistema que controla la logica de los demonios activos en partida
 */
UCLASS(Blueprintable)
class SN_PROJECT_API USN_DemonSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:

	ISN_Spawner* ActiveSpawner;
	int CurrentSpawnerIndex = 0;

	APlayerController* PlayerControllerRef;

	bool bEnableDemonSpawn;

	FTimerHandle CameraTimerHandle;
	FTimerHandle SpawnTimerHandle;

	TArray<ASN_AbstractDemon*> aDemonsToMovement;
	TMap<AActor*, TArray<ASN_AbstractDemon*>> MapDemonsToAtack;
	int CurrentFloorAtack = 0;
	TArray<AActor*> aKeysToAttack;

	FTimerHandle InitAttackTimerHandle;
	FTimerHandle AttackTimerHandle;
	FTimerHandle SecondWaveTimerHandle;
	FTimerDelegate AttackTimerDelegate;
	bool bAttackTurnActive;
	
	//Controla si es la primera o la segunda oleada
	int CountAttackWave = 1;


	
protected:
	
	int CountSpawns;

	int GameLevel;

	TArray<ISN_Spawner*> aSpawnDemons;

	TArray<AActor*> aCharacters;

	TArray<ASN_AbstractDemon*> aDemons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	TArray<FCardsDemon> aCardsDemons;

	int CountActiveDemonsThisTurn;

public:
	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnDemonDeathDelegate DemonDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnDemonsTurnEndDelegate DemonsTurnEndDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnAllDemonsEndActionDelegate AllDemonsEndActionDelegate;
	
protected:
	
	UFUNCTION()
	void DecrementDemonsInAction();

	void SearchAllDemonsSpawnInWorld();

	UFUNCTION()
	void ReturnCameraToPlayer();

	UFUNCTION()
	void BlendCameraToActiveSpawner();

	UFUNCTION()
	void InitAttackInFloor();

	void InitDemonsMovement();

	UFUNCTION()
	void RealizeAttackInFloor(TArray<ASN_AbstractDemon*> aDemonsToAttack);

	FSpawnStruct CreateSpawnStruct(FCardsDemon DemonCard);
	
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void InitDemonSpawnRef();

	void ActiveRandomSpawn();

	void InitCards(UDataTable* DemonsDataTable);

	void AddCharacter(AActor* NewCharacter);

	UFUNCTION(BlueprintCallable)
	void ActiveDemonsTurn(bool ExecuteDemonSpawn = true, TSubclassOf<ASN_AbstractDemon> DoubleActivationType = nullptr);

	void ActiveDemonsTurnFunctionDelegate();
	
	UFUNCTION()
	void InitSpawnDemons();
	
	UFUNCTION()
	void SpawnDemonsInActiveSpawner();

	void SpawnDemonsInFloors(TArray<AActor*> aActorsFloors);

	void SpawnDemonsInFloor(IIMovement* ActorsFloors, bool bActiveInitAnimaion);

	void SpawnDemon(FVector SpawnLocation, TSubclassOf<ASN_AbstractDemon> DemonClass, bool bActiveInitAnimation = true);

	FCardsDemon GetRandomCard();

	/*
	 * Reinicia las cartas de demonios una vez gastadas todas.
	 */
	void ResetCards();

	/*
	 * Inicia el subsistema recibiendo el DataTable con todas las cartas de spawn de demonios
	 */
	UFUNCTION(BlueprintCallable)
	void InitSubsystem(UDataTable* DemonsDataTable);

	AActor* GetTargetFloor();

	UFUNCTION()
	void DemonDeath(int DeathExp);

	UFUNCTION()
	void AddNewGameLevel(int NewGameLevel);
	
	UFUNCTION(BlueprintPure, Category="SN")
	TArray<FDemonGameSave> CreateDataDemonsToSave();

	void ActiveSpawn();

	
	
	
};

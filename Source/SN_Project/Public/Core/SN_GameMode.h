// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/CardsItems.h"
#include "GameFramework/GameMode.h"
#include "SaveGame/CharactersGameSave.h"
#include "SaveGame/DemonGameSave.h"
#include "SN_GameMode.generated.h"

/**
 * 
 */

class ASN_Objective;
class IObjetivable;
class ASN_Character;
class USN_DemonSubsystem;
class IIMovement;
class ASN_ExitCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameFinishedDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterExpUpDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDemonTurnInitDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDemonTurnEndDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitCharacterTurnDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameLevelUpDelegate);

UCLASS()
class SN_PROJECT_API ASN_GameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int GameLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int MaxCharacterExp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int MaxObjectivesLevel;

	int CurrentObjetivesLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int IdCharacterActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int MaxNumberCharacters;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	TSubclassOf<class ASN_Character> CharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	TArray<ASN_Character*> aCharacters;

	TArray<IIMovement*> aFloors;

	USN_DemonSubsystem* DemonSubsystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	TArray<FCardsItems> aItems;

	TArray<FCardsItems> aInitialItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Items")
	UDataTable* ItemsDataTable;

	bool GameFinished;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bDeleteFakeObjective;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bActiveGameLevelCustom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	int CustomExpCharacter;

	TArray<ASN_Objective*> aActorsObjectives;

	ASN_Objective* ObjectiveExitCharacterRef;

public:
	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnGameFinishedDelegate FOnGameFinishedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnCharacterExpUpDelegate CharacterExpUpDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnDemonTurnInitDelegate DemonTurnInitDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnDemonTurnEndDelegate DemonTurnEndDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnInitCharacterTurnDelegate InitCharacterTurnDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnGameLevelUpDelegate GameLevelUpDelegate;
	
public:
	// Sets default values for this actor's properties
	ASN_GameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="SN")
	void InitCharacters();

	void InitInitialWeapons();

	UFUNCTION(BlueprintCallable, Category="SN")
	void InitPlayer();

	void InitFloors();

	UFUNCTION()
	void DemonsEndTurn();

	void InitObjectives();

	void DeleteObjectives();

	bool ActiveObjective();

	UFUNCTION()
	void TakeObjective(int32 ExpValue);

	UFUNCTION()
	void ExitCharacter(int32 ExpValue);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SaveGame();

	UFUNCTION(BlueprintPure, Category="SN")
	TArray<FCharactersGameSave> CreateDataCharactersToSave();

	UFUNCTION(BlueprintPure, Category="SN")
	TArray<FDemonGameSave> CreateDataDemonsToSave();
	
public:
	UFUNCTION(BlueprintCallable, Category="SN")
	void InitCharacterTurn();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="SN")
	ASN_Character* GetActiveCharacter() const;

	UFUNCTION(BlueprintCallable)
	void ResetAllFloors();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category="Item")
	FCardsItems GetRandomItem();

	void InitItems();

	void ResetAllItems();

	UFUNCTION()
	void CharacterDeath();

	UFUNCTION()
	void CharacterLevelUp();

	
	bool CheckWin();

	void FinishGame(bool bGameWin);

	UFUNCTION(BlueprintImplementableEvent, Category="SN")
	void BP_FinishGame(bool bGameWin);

	UFUNCTION()
	void AddExpActiveCharacter(int NewExp);

	void CheakGameLevel(int CharacterExp);

	TArray<ASN_Character*> GetCharacters() { return aCharacters; }

	void VerifyContinueGame();

	FCardsItems GetCardItemById(int ItemId);

	UFUNCTION()
	void InitCharacterContinueGame(TArray<FCharactersGameSave> aSaveData);

	UFUNCTION()
	void InitDemonsContinueGame(TArray<FDemonGameSave> aSaveData);

	bool IsGameFinished(){ return GameFinished; }
	
};

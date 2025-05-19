// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SaveGame/CharactersGameSave.h"
#include "SaveGame/DemonGameSave.h"
#include "SN_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SN_PROJECT_API USN_GameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	bool bContinueGame;

	TArray<FCharactersGameSave> aCharactersGameSave;

	TArray<FDemonGameSave> aDemonsGameSave;

public:

	/*
	 * Indicador para el GameMode para saber si la partida que empieza es nueva o un continue
	 */
	UFUNCTION(BlueprintPure)
	bool IsContinueGame() { return bContinueGame; }

	UFUNCTION(BlueprintCallable)
	void SetContinueGame(bool bNewContinueGame) { bContinueGame = bNewContinueGame; }

	/*
	 * Devuelve la información guardada de la anterior partida de los Characters
	 */
	UFUNCTION(BlueprintPure)
	TArray<FCharactersGameSave> GetCharactersGameSave() { return aCharactersGameSave; }

	/*
	 * Setea la información acual de los characters para realizar el Save
	 */
	UFUNCTION(BlueprintCallable)
	void SetCharactersSaveGame(TArray<FCharactersGameSave> aNewGameSave)
	{
		aCharactersGameSave = aNewGameSave;
	}

	/*
	 * Devuelve la información guardada de la anterior partida de los Demonios
	 */
	UFUNCTION(BlueprintPure)
	TArray<FDemonGameSave> GetDemonsGameSave() { return aDemonsGameSave; }

	/*
	 * Setea la información acual de los demonios para realizar el Save
	 */
	UFUNCTION(BlueprintCallable)
	void SetDemonsSaveGame(TArray<FDemonGameSave> aNewGameSave)
	{
		aDemonsGameSave = aNewGameSave;
	}
	
};

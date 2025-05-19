// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SN_GameMode.h"

#include "Actors/IMovement.h"
#include "Actors/SN_InitCharacters.h"
#include "Actors/SN_Objective.h"
#include "Characters/SN_Character.h"
#include "Core/SN_GameInstance.h"
#include "Demons/SN_DemonSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Library/SN_Library.h"

ASN_GameMode::ASN_GameMode()
{
	MaxCharacterExp = 0;
	GameLevel = 1;
	MaxObjectivesLevel = 2;
	MaxNumberCharacters = 6;
	IdCharacterActive = -1;
	GameFinished = false;

	bDeleteFakeObjective = false;
	bActiveGameLevelCustom = false;
	CustomExpCharacter = 0;
}

void ASN_GameMode::BeginPlay()
{
	Super::BeginPlay();

	DemonSubsystem = UWorld::GetSubsystem<USN_DemonSubsystem>(GetWorld());

	if (IsValid(DemonSubsystem))
	{
		DemonSubsystem->DemonDeathDelegate.AddDynamic(this, &ThisClass::AddExpActiveCharacter);
		DemonSubsystem->DemonsTurnEndDelegate.AddDynamic(this, &ThisClass::DemonsEndTurn);
	}

	InitObjectives();
	InitItems();
	InitCharacters();
	InitInitialWeapons();

	InitFloors();

	VerifyContinueGame();

	InitCharacterTurn();

	if (bActiveGameLevelCustom)
	{
		CheakGameLevel(CustomExpCharacter);
	}
}

void ASN_GameMode::InitFloors()
{
	TArray<AActor*> aTmpActors;

	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UIMovement::StaticClass(), aTmpActors);

	for (AActor* Actor : aTmpActors)
	{
		if (IIMovement* Floor = Cast<IIMovement>(Actor))
		{
			aFloors.Add(Floor);
		}
	}
}

void ASN_GameMode::InitCharacters()
{
	AActor* InitCharacterLocation = UGameplayStatics::GetActorOfClass(GetWorld(), ASN_InitCharacters::StaticClass());

	if (!InitCharacterLocation)
	{
		return;
	}

	AActor* StartActor = USN_Library::GetActiveActorFloor(InitCharacterLocation);

	IIMovement* StartFloor = Cast<IIMovement>(StartActor);

	if (!StartFloor)
	{
		return;
	}

	for (int i = 0; i < MaxNumberCharacters; i++)
	{
		FVector SpawnLocation = StartFloor->GetRandomLocationInFloor();

		//Se pone +100 para evitar que atraviese el suelo
		SpawnLocation.Z += 100;

		ASN_Character* Character = GetWorld()->SpawnActor<ASN_Character>(
			CharacterClass, SpawnLocation, InitCharacterLocation->GetActorRotation());

		if (Character)
		{
			Character->FOnCharacterDeathDelegate.AddDynamic(this, &ThisClass::CharacterDeath);
			Character->CharacterLevelUpDelegate.AddDynamic(this, &ThisClass::CharacterLevelUp);

			aCharacters.Add(Character);
			DemonSubsystem->AddCharacter(Character);
		}
	}
}

void ASN_GameMode::InitInitialWeapons()
{
	for (ASN_Character* Character : aCharacters)
	{
		bool SearchWeapon = true;

		while (SearchWeapon)
		{
			int RandomNumber = FMath::RandRange(0, aInitialItems.Num() - 1);

			FCardsItems& Item = aInitialItems[RandomNumber];

			if (Item.NumInitWeapon > 0)
			{
				SearchWeapon = false;
				Item.NumInitWeapon--;
				Character->AddItem(true, Item);
				Character->AttachWeaponCard(Character->GetLeftItem(), true);				
			}
		}
	}
}

ASN_Character* ASN_GameMode::GetActiveCharacter() const
{
	if (IdCharacterActive < 0 || IdCharacterActive >= aCharacters.Num())
	{
		return nullptr;
	}

	return aCharacters[IdCharacterActive];
}

void ASN_GameMode::ResetAllFloors()
{
	for (IIMovement* Floor : aFloors)
	{
		Floor->EnableDisableMovement(false);
	}
}

FCardsItems ASN_GameMode::GetRandomItem()
{
	int Max = aItems.Num() - 1;

	for (int CurrentAttempts = 0; CurrentAttempts < aItems.Num(); CurrentAttempts++)
	{
		int RandomNumber = FMath::RandRange(0, Max);

		FCardsItems& Item = aItems[RandomNumber];

		if (Item.CurrentItems > 0)
		{
			Item.CurrentItems--;
			return Item;
		}
	}

	ResetAllItems();

	return GetRandomItem();
}

void ASN_GameMode::InitItems()
{
	if (ItemsDataTable)
	{
		TArray<FCardsItems*> TempItems;
		ItemsDataTable->GetAllRows<FCardsItems>(TEXT("Context"), TempItems);

		for (const FCardsItems* Item : TempItems)
		{
			if (Item)
			{
				aItems.Add(*Item);

				if (Item->bInitWeapon)
				{
					aInitialItems.Add(*Item);
				}
			}
		}
	}
}

void ASN_GameMode::ResetAllItems()
{
	for (FCardsItems& Item : aItems)
	{
		Item.CurrentItems = Item.InitItems;
	}
}

void ASN_GameMode::CharacterDeath()
{
	bool AllDeaths = true;

	for (ASN_Character* Character : aCharacters)
	{
		if (!Character->IsDeath())
		{
			AllDeaths = false;
			break;
		}
	}

	if (AllDeaths)
	{
		FinishGame(false);
	}
}

void ASN_GameMode::CharacterLevelUp()
{
	USN_Library::ShowMessageToPlayer(GetWorld(), TEXT("Character Level UP"));
}

bool ASN_GameMode::CheckWin()
{
	bool WinGame = true;

	for (ASN_Character* Character : aCharacters)
	{
		if (!Character->IsEscape())
		{
			WinGame = false;
			break;
		}
	}

	if (WinGame)
	{
		FinishGame(true);
		return true;
	}

	return false;
}

void ASN_GameMode::FinishGame(bool bGameWin)
{
	GameFinished = true;
	FOnGameFinishedDelegate.Broadcast();
	BP_FinishGame(bGameWin);
}

void ASN_GameMode::InitPlayer()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PlayerController)
	{
		return;
	}

	FInputModeGameAndUI InputMode;

	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void ASN_GameMode::DemonsEndTurn()
{
	for (ASN_Character* Character : aCharacters)
	{
		Character->SetInvisible(false);
	}

	IdCharacterActive = -1;
	InitCharacterTurn();
	DemonTurnEndDelegate.Broadcast();

	if (!GameFinished)
	{
		BP_SaveGame();
	}
}

void ASN_GameMode::InitObjectives()
{
	TArray<AActor*> TempActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASN_Objective::StaticClass(), TempActors);

	int NumObjActives = 0;

	for (AActor* Actor : TempActors)
	{
		ASN_Objective* Objective = Cast<ASN_Objective>(Actor);

		if (!Objective)
		{
			continue;
		}

		if (Objective->IsExit())
		{
			ObjectiveExitCharacterRef = Objective;
			ObjectiveExitCharacterRef->ObjectiveTakeDelegate.AddUniqueDynamic(this, &ThisClass::ExitCharacter);
		}
		else
		{
			aActorsObjectives.Add(Objective);
		}
	}

	if (!aActorsObjectives.IsEmpty())
	{
		while (NumObjActives < MaxObjectivesLevel)
		{
			if (ActiveObjective())
			{
				++NumObjActives;
			}
		}
	}

	if (bDeleteFakeObjective)
	{
		for (ASN_Objective* Object : aActorsObjectives)
		{
			if (!Object->IsRealObjective())
			{
				AActor* Objective = Cast<AActor>(Object);
				Objective->Destroy();
			}
		}
	}
}

void ASN_GameMode::DeleteObjectives()
{
	AActor* ExitActor = Cast<AActor>(GetWorld()->GetFirstPlayerController());

	for (ASN_Objective* Actor : aActorsObjectives)
	{
		if (Actor && (!Actor->IsExit()))
		{
			Actor->Enable(false);
		}
	}
}

bool ASN_GameMode::ActiveObjective()
{
	if (aActorsObjectives.IsEmpty())
	{
		return false;
	}

	int RandomNumber = FMath::RandRange(0, aActorsObjectives.Num() - 1);

	ASN_Objective* Objective = aActorsObjectives[RandomNumber];

	if (!Objective || (Objective->IsRealObjective()))
	{
		return false;
	}

	Objective->SetRealObjective(true);
	Objective->ObjectiveTakeDelegate.AddUniqueDynamic(this, &ThisClass::TakeObjective);

	return true;
}

void ASN_GameMode::TakeObjective(int32 ExpValue)
{
	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return;
	}

	Character->AddExp(ExpValue);
	CheakGameLevel(Character->GetExp());

	++CurrentObjetivesLevel;

	if (CurrentObjetivesLevel == MaxObjectivesLevel && ObjectiveExitCharacterRef)
	{
		ObjectiveExitCharacterRef->Enable(true);
		DeleteObjectives();
		USN_Library::ShowMessageToPlayer(GetWorld(), "Se ha activado la salida!");
	}

	DemonSubsystem->ActiveRandomSpawn();
}

void ASN_GameMode::ExitCharacter(int32 ExpValue)
{
	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return;
	}

	Character->SetEscape(true);
	Character->SetActorHiddenInGame(true);
	USN_Library::ShowMessageToPlayer(GetWorld(), "El peligro ha quedado atrás… por ahora.");

	if (!CheckWin())
	{
		InitCharacterTurn();
	}
}

TArray<FCharactersGameSave> ASN_GameMode::CreateDataCharactersToSave()
{
	TArray<FCharactersGameSave> aSaveData;

	for (ASN_Character* Character : aCharacters)
	{
		FCharactersGameSave DataGameSave;
		DataGameSave.Location = Character->GetActorLocation();
		DataGameSave.Rotation = Character->GetActorRotation();
		DataGameSave.CurrentHealth = Character->GetHealth();
		DataGameSave.CurrentExp = Character->GetExp();
		DataGameSave.bIsDead = Character->IsDeath();

		if (Character->IsLeftItemInit())
		{
			DataGameSave.LeftItemId = Character->GetLeftItem().Id;
		}

		if (Character->IsRightItemInit())
		{
			DataGameSave.RightItemId = Character->GetRightItem().Id;
		}

		if (Character->IsBodyItemInit())
		{
			DataGameSave.BodyItemId = Character->GetBodyItem().Id;
		}

		aSaveData.Add(DataGameSave);
	}

	return aSaveData;
}

TArray<FDemonGameSave> ASN_GameMode::CreateDataDemonsToSave()
{
	return DemonSubsystem->CreateDataDemonsToSave();
}

void ASN_GameMode::InitCharacterTurn()
{
	if (GameFinished)
	{
		return;
	}

	//Disable old Character
	ASN_Character* Character = GetActiveCharacter();

	if (Character)
	{
		Character->EnableDisableShowPosition(false);
	}

	IdCharacterActive++;

	if (IdCharacterActive == MaxNumberCharacters)
	{
		DemonTurnInitDelegate.Broadcast();
		DemonSubsystem->ActiveDemonsTurn();
		//IdCharacterActive = 0;
		return;
	}

	if (GameFinished)
	{
		return;
	}

	//Enable new Character
	Character = GetActiveCharacter();

	if (!Character || (Character->IsDeath() || Character->IsEscape()))
	{
		InitCharacterTurn();
		return;
	}

	Character->EnableDisableShowPosition(true);

	InitCharacterTurnDelegate.Broadcast();
}

void ASN_GameMode::AddExpActiveCharacter(int NewExp)
{
	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return;
	}

	Character->AddExp(NewExp);

	CheakGameLevel(Character->GetExp());
}

void ASN_GameMode::CheakGameLevel(int CharacterExp)
{
	if (GameLevel == 4)
	{
		return;
	}

	if (MaxCharacterExp < CharacterExp)
	{
		MaxCharacterExp = CharacterExp;
	}

	int NewGameLevel = 1;

	if (CharacterExp >= 43)
	{
		NewGameLevel = 4;
	}
	else if (CharacterExp >= 19)
	{
		NewGameLevel = 3;
	}
	else if (CharacterExp >= 7)
	{
		NewGameLevel = 2;
	}

	if (NewGameLevel > GameLevel)
	{
		GameLevel = NewGameLevel;
		DemonSubsystem->AddNewGameLevel(GameLevel);
		GameLevelUpDelegate.Broadcast();
	}

	CharacterExpUpDelegate.Broadcast();
}

void ASN_GameMode::VerifyContinueGame()
{
	USN_GameInstance* GameInstance = Cast<USN_GameInstance>(GetGameInstance());

	if (!IsValid(GameInstance))
	{
		return;
	}

	if (GameInstance->IsContinueGame())
	{
		InitCharacterContinueGame(GameInstance->GetCharactersGameSave());
		InitDemonsContinueGame(GameInstance->GetDemonsGameSave());
	}
}

FCardsItems ASN_GameMode::GetCardItemById(int ItemId)
{
	FCardsItems FindItem = FCardsItems();

	for (FCardsItems Item : aItems)
	{
		if (ItemId == Item.Id)
		{
			FindItem = Item;
			break;
		}
	}

	return FindItem;
}


void ASN_GameMode::InitCharacterContinueGame(TArray<FCharactersGameSave> aSaveData)
{
	if (!aSaveData.IsEmpty())
	{
		for (int i = 0; i < aSaveData.Num(); ++i)
		{
			FCharactersGameSave CharacterGameSave = aSaveData[i];
			ASN_Character* Character = aCharacters[i];

			if (!IsValid(Character))
			{
				continue;
			}

			Character->SetActorLocation(CharacterGameSave.Location);
			Character->SetActorRotation(CharacterGameSave.Rotation);
			Character->SetHealth(CharacterGameSave.CurrentHealth);
			Character->SetExp(CharacterGameSave.CurrentExp);

			Character->ClearAllItems();

			if (CharacterGameSave.LeftItemId != -1)
			{
				Character->AddItem(true, GetCardItemById(CharacterGameSave.LeftItemId));
			}

			if (CharacterGameSave.RightItemId != -1)
			{
				Character->AddItem(false, GetCardItemById(CharacterGameSave.RightItemId));
			}

			if (CharacterGameSave.BodyItemId != -1)
			{
				Character->AddBodyItem(GetCardItemById(CharacterGameSave.BodyItemId));
			}

			if (CharacterGameSave.bIsDead)
			{
				Character->Death();
			}

			Character->UpdateHudHealth();
			CheakGameLevel(Character->GetExp());
		}
	}
}

void ASN_GameMode::InitDemonsContinueGame(TArray<FDemonGameSave> aSaveData)
{
	if (!aSaveData.IsEmpty())
	{
		for (int i = 0; i < aSaveData.Num(); ++i)
		{
			FDemonGameSave DemonGameSave = aSaveData[i];
			//DemonSubsystem->SpawnDemons(DemonGameSave.Location, DemonGameSave.Rotation, DemonGameSave.DemonClass, 1,
			  //                          false);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Demons/SN_DemonSubsystem.h"

#include "NavigationSystem.h"
#include "Actors/IMovement.h"
#include "Demons/SN_AbstractDemon.h"
#include "Demons/SN_SpawnDemon.h"
#include "Kismet/GameplayStatics.h"
#include "Library/SN_Library.h"
#include "SN_Components/HealthInterface.h"

void USN_DemonSubsystem::DecrementDemonsInAction()
{
	--CountActiveDemonsThisTurn;

	if (CountActiveDemonsThisTurn <= 0 && bEnableDemonSpawn)
	{
		if (bAttackTurnActive)
		{
			GetWorld()->GetTimerManager().SetTimer(InitAttackTimerHandle, this, &USN_DemonSubsystem::InitAttackInFloor,
			                                       1.f, false);
		}
	}
}

void USN_DemonSubsystem::SearchAllDemonsSpawnInWorld()
{
	TArray<AActor*> aTmpActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASN_AbstractDemon::StaticClass(), aTmpActors);

	for (AActor* Actor : aTmpActors)
	{
		ASN_AbstractDemon* Demon = Cast<ASN_AbstractDemon>(Actor);

		if (IsValid(Demon))
		{
			aDemons.Add(Demon);
			Demon->DeathDemonDelegate.AddUniqueDynamic(this, &ThisClass::DemonDeath);
			Demon->DemonEndActionsDelegate.AddUniqueDynamic(this, &ThisClass::DecrementDemonsInAction);
		}
	}
}

void USN_DemonSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CountActiveDemonsThisTurn = 0;
	CountSpawns = 1;

	this->AllDemonsEndActionDelegate.AddDynamic(this, &ThisClass::InitSpawnDemons);
}

void USN_DemonSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USN_DemonSubsystem::InitDemonSpawnRef()
{
	TArray<AActor*> aActorsSpawn;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASN_SpawnDemon::StaticClass(), aActorsSpawn);

	if (aActorsSpawn.IsEmpty())
	{
		return;
	}

	for (AActor* SpawnActor : aActorsSpawn)
	{
		if (ISN_Spawner* Spawner = Cast<ISN_Spawner>(SpawnActor))
		{
			aSpawnDemons.Add(Spawner);
		}
	}
}

void USN_DemonSubsystem::ActiveRandomSpawn()
{
	TArray<ISN_Spawner*> aNoActiveSpawns;

	for (ISN_Spawner* Spawner : aSpawnDemons)
	{
		if (!Spawner->ISEnabled())
		{
			aNoActiveSpawns.Add(Spawner);
		}
	}

	if (aNoActiveSpawns.IsEmpty())
	{
		return;
	}
	if (ISN_Spawner* Spawner = aNoActiveSpawns[FMath::RandRange(0, aNoActiveSpawns.Num() - 1)])
	{
		ASN_SpawnDemon* SpanDemon = Cast<ASN_SpawnDemon>(Spawner);

		if (IsValid(SpanDemon))
		{
			if (PlayerControllerRef)
			{
				PlayerControllerRef->SetViewTargetWithBlend(SpanDemon, 2.5f);
				ActiveSpawner = Spawner;

				if (const UWorld* World = GetWorld())
				{
					FTimerHandle TimerHandle;
					World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::ActiveSpawn, 1.f, false);
				}
				else
				{
					ActiveSpawner->Active();
					ActiveSpawner = nullptr;
				}
			}
		}
	}
}

void USN_DemonSubsystem::InitCards(UDataTable* DemonsDataTable)
{
	if (DemonsDataTable)
	{
		TArray<FCardsDemon*> TempCards;
		DemonsDataTable->GetAllRows<FCardsDemon>(TEXT("Context"), TempCards);

		for (const FCardsDemon* Card : TempCards)
		{
			if (Card)
			{
				aCardsDemons.Add(*Card);
			}
		}
	}
}

void USN_DemonSubsystem::AddCharacter(AActor* NewCharacter)
{
	aCharacters.Add(NewCharacter);
}

void USN_DemonSubsystem::ActiveDemonsTurn(bool ExecuteDemonSpawn, TSubclassOf<ASN_AbstractDemon> DoubleActivationType)
{
	bEnableDemonSpawn = ExecuteDemonSpawn;

	CountActiveDemonsThisTurn = 0;
	CurrentFloorAtack = 0;

	aKeysToAttack.Empty();
	MapDemonsToAtack.Empty();
	aDemonsToMovement.Empty();

	bAttackTurnActive = true;

	for (AActor* Character : aCharacters)
	{
		IHealthInterface* HealthCharacter = Cast<IHealthInterface>(Character);

		if (!HealthCharacter)
		{
			continue;
		}

		HealthCharacter->ClearCountEnemiesAttackMe();
	}

	for (ASN_AbstractDemon* Demon : aDemons)
	{
		if (!IsValid(Demon))
		{
			UE_LOG(LogTemp, Error, TEXT("Demon no es válido (posiblemente fue eliminado)."));
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("Demon es válido, verificando IsDeath()."));

		if (Demon->IsDeath())
		{
			UE_LOG(LogTemp, Warning, TEXT("Demon está muerto."));
			continue;
		}

		if (!Demon->IsActive())
		{
			continue;
		}

		if (IsValid(DoubleActivationType) && !Demon->IsA(DoubleActivationType))
		{
			UE_LOG(LogTemp, Warning, TEXT("DoubleActivationType"));
			continue;
		}

		if (CountAttackWave == 2 && Demon->GetNumActions() == 1)
		{
			continue;
		}

		++CountActiveDemonsThisTurn;

		Demon->InicializeDemonValues();

		if (Demon->ExistCharacterToAttack())
		{
			AActor* FloorDemon = USN_Library::GetActiveActorFloor(Demon);

			if (IsValid(FloorDemon))
			{
				if (!MapDemonsToAtack.Contains(FloorDemon))
				{
					TArray<ASN_AbstractDemon*> NewList;
					MapDemonsToAtack.Add(FloorDemon, NewList);
				}

				MapDemonsToAtack[FloorDemon].Add(Demon);
			}
		}
		else
		{
			aDemonsToMovement.Add(Demon);
		}
	}

	MapDemonsToAtack.GetKeys(aKeysToAttack);

	GetWorld()->GetTimerManager().SetTimer(InitAttackTimerHandle, this, &USN_DemonSubsystem::InitAttackInFloor,
	                                       .5f, false);
}

void USN_DemonSubsystem::ActiveDemonsTurnFunctionDelegate()
{
	ActiveDemonsTurn(true, nullptr);
}

void USN_DemonSubsystem::InitSpawnDemons()
{
	CurrentSpawnerIndex = 0;
	BlendCameraToActiveSpawner();
//	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, TEXT("InitSpawnDemons"));
}


void USN_DemonSubsystem::BlendCameraToActiveSpawner()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

	if (CurrentSpawnerIndex >= aSpawnDemons.Num())
	{
		ReturnCameraToPlayer();
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, FString::FromInt(CurrentSpawnerIndex));

	ISN_Spawner* Spawner = aSpawnDemons[CurrentSpawnerIndex];

	if (!Spawner)
	{
		ReturnCameraToPlayer();
		return;
	}

	if (!Spawner->ISEnabled())
	{
		CurrentSpawnerIndex++;
		BlendCameraToActiveSpawner();
		return;
	}

	if (IsValid(PlayerControllerRef))
	{
		AActor* SpawnActor = Cast<AActor>(Spawner);
		PlayerControllerRef->SetViewTargetWithBlend(SpawnActor, 2.5f);
	}

	if (CountSpawns > 1)
	{
		Spawner->ActiveSuperSpawn();
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &USN_DemonSubsystem::SpawnDemonsInActiveSpawner,
	                                       3.f, false);
}

void USN_DemonSubsystem::InitAttackInFloor()
{
	if (CurrentFloorAtack >= aKeysToAttack.Num())
	{
		bAttackTurnActive = false;
		InitDemonsMovement();

		if (CountAttackWave == 1)
		{
			++CountAttackWave;
			GetWorld()->GetTimerManager().SetTimer(SecondWaveTimerHandle, this,
			                                       &USN_DemonSubsystem::ActiveDemonsTurnFunctionDelegate,
			                                       1.f, false);
		}
		else
		{
			CountAttackWave = 1;
			AllDemonsEndActionDelegate.Broadcast();
		}

		return;
	}

	AActor* FloorActor = aKeysToAttack[CurrentFloorAtack];

	PlayerControllerRef->SetViewTargetWithBlend(FloorActor, 1.0f);

	AttackTimerDelegate.BindUFunction(this, FName("RealizeAttackInFloor"), MapDemonsToAtack[FloorActor]);
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, AttackTimerDelegate, 1.2f, false);
}

void USN_DemonSubsystem::InitDemonsMovement()
{
	AActor* TargetActor = GetTargetFloor();

	if (!IsValid(TargetActor))
	{
		return;
	}

	for (ASN_AbstractDemon* Demon : aDemonsToMovement)
	{
		Demon->Movement(TargetActor, aCharacters);
	}
}

void USN_DemonSubsystem::RealizeAttackInFloor(TArray<ASN_AbstractDemon*> aDemonsToAttack)
{
	GetWorld()->GetTimerManager().ClearTimer(InitAttackTimerHandle);

	CountActiveDemonsThisTurn = aDemonsToAttack.Num();

	for (ASN_AbstractDemon* Demon : aDemonsToAttack)
	{
		Demon->Attack();
	}

	++CurrentFloorAtack;
}


void USN_DemonSubsystem::SpawnDemonsInFloors(TArray<AActor*> aActorsFloors)
{
	for (AActor* ActorFloor : aActorsFloors)
	{
		IIMovement* Floor = Cast<IIMovement>(ActorFloor);
		SpawnDemonsInFloor(Floor, false);
	}
}

void USN_DemonSubsystem::SpawnDemonsInFloor(IIMovement* Floor, bool bActiveInitAnimaion)
{
	if (!Floor || (!Floor->CanSpawnDemonsInFloor()))
	{
		return;
	}

	int NewSpawnCount = 1;

	for (int j = 0; j < CountSpawns; j++)
	{
		FCardsDemon Card = GetRandomCard();

		if (Card.bDoubleSpawn)
		{
			++NewSpawnCount;
			continue;
		}

		FSpawnStruct SpawnStruct = CreateSpawnStruct(Card);

		for (int i = 0; i < SpawnStruct.NumDemonsSpawned; ++i)
		{
			SpawnDemon(Floor->GetRandomLocationInFloor(), SpawnStruct.DemonClass, bActiveInitAnimaion);
		}
	}

	CountSpawns = NewSpawnCount;
}

FSpawnStruct USN_DemonSubsystem::CreateSpawnStruct(FCardsDemon DemonCard)
{
	FSpawnStruct SpawnStruct;

	switch (GameLevel)
	{
	case 1:
		{
			SpawnStruct.NumDemonsSpawned = DemonCard.level_1_num;
			SpawnStruct.DemonClass = DemonCard.level_1_type;
			break;
		}
	case 2:
		{
			SpawnStruct.NumDemonsSpawned = DemonCard.level_2_num;
			SpawnStruct.DemonClass = DemonCard.level_2_type;
			break;
		}
	case 3:
		{
			SpawnStruct.NumDemonsSpawned = DemonCard.level_3_num;
			SpawnStruct.DemonClass = DemonCard.level_3_type;
			break;
		}
	case 4:
		{
			SpawnStruct.NumDemonsSpawned = DemonCard.level_4_num;
			SpawnStruct.DemonClass = DemonCard.level_4_type;
			break;
		}
	}

	return SpawnStruct;
}

void USN_DemonSubsystem::SpawnDemonsInActiveSpawner()
{
	ISN_Spawner* Spawner = aSpawnDemons[CurrentSpawnerIndex];

	int NewSpawnCount = 1;

	bool bExistNewDemons = false;

	for (int j = 0; j < CountSpawns; j++)
	{
		FCardsDemon Card = GetRandomCard();

		if (Card.bDoubleSpawn)
		{
			++NewSpawnCount;
			continue;
		}

		FSpawnStruct SpawnStruct = CreateSpawnStruct(Card);

		for (int i = 0; i < SpawnStruct.NumDemonsSpawned; ++i)
		{
			SpawnDemon(Spawner->GetSpawnLocation(), SpawnStruct.DemonClass, true);
		}

		bExistNewDemons = true;
	}

	CountSpawns = NewSpawnCount;

	float TimeDelay = .01f;

	if (bExistNewDemons)
	{
		TimeDelay = 1.5f;
	}

	CountSpawns = NewSpawnCount;

	++CurrentSpawnerIndex;
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &USN_DemonSubsystem::BlendCameraToActiveSpawner,
	                                       TimeDelay, false);
}

void USN_DemonSubsystem::SpawnDemon(FVector SpawnLocation,
                                    TSubclassOf<ASN_AbstractDemon> DemonClass,
                                    bool bActiveInitAnimation)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSystem)
	{
		return;
	}

	FNavLocation NavLocation;
	if (!NavSystem->ProjectPointToNavigation(SpawnLocation, NavLocation))
	{
		return;
	}

	ASN_AbstractDemon* Demon = GetWorld()->SpawnActor<ASN_AbstractDemon>(
		DemonClass, SpawnLocation, FRotator(0, FMath::RandRange(-180, 180), 0));

	if (bActiveInitAnimation)
	{
		Demon->PlaySpawnMontage();
		Demon->PlaySpawnParticles();
	}

	if (Demon)
	{
		aDemons.Add(Demon);
		Demon->DeathDemonDelegate.AddUniqueDynamic(this, &ThisClass::DemonDeath);
		Demon->DemonEndActionsDelegate.AddUniqueDynamic(this, &ThisClass::DecrementDemonsInAction);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ERROR SPAWN DEMON"));
	}
}

FCardsDemon USN_DemonSubsystem::GetRandomCard()
{
	const int Max = aCardsDemons.Num() - 1;

	for (int CurrentAttempts = 0; CurrentAttempts < aCardsDemons.Num(); CurrentAttempts++)
	{
		int RandomNumber = FMath::RandRange(0, Max);

		FCardsDemon& Card = aCardsDemons[RandomNumber];

		if (Card.bEnable)
		{
			Card.bEnable = false;
			return Card;
		}
	}

	ResetCards();

	return GetRandomCard();
}

void USN_DemonSubsystem::ResetCards()
{
	UE_LOG(LogTemp, Warning, TEXT("USN_DemonSubsystem - ResetCards"));

	for (int i = 0; i < aCardsDemons.Num(); i++)
	{
		FCardsDemon& Card = aCardsDemons[i];
		Card.bEnable = true;
	}
}

void USN_DemonSubsystem::InitSubsystem(UDataTable* DemonsDataTable)
{
	UE_LOG(LogTemp, Warning, TEXT("USN_DemonSubsystem - InitSubsystem"));

	GameLevel = 1;

	InitDemonSpawnRef();
	InitCards(DemonsDataTable);
	SearchAllDemonsSpawnInWorld();

	PlayerControllerRef = UGameplayStatics::GetPlayerController(this, 0);
}

AActor* USN_DemonSubsystem::GetTargetFloor()
{
	TMap<AActor*, int> TargetMap;

	for (AActor* Character : aCharacters)
	{
		IHealthInterface* HealthCharacter = Cast<IHealthInterface>(Character);

		if (!HealthCharacter || (HealthCharacter->IsDeath()))
		{
			continue;
		}

		AActor* ActiveFloor = USN_Library::GetActiveActorFloor(Character);

		if (!ActiveFloor)
		{
			continue;
		}

		if (!TargetMap.Contains(ActiveFloor))
		{
			TargetMap.Add(ActiveFloor, 0);
		}

		TargetMap.Add(ActiveFloor, TargetMap[ActiveFloor] + 1);
	}

	int CountCharacter = 0;
	AActor* FloorReturn = nullptr;

	for (const TPair<AActor*, int32>& Pair : TargetMap)
	{
		if (Pair.Value > CountCharacter)
		{
			CountCharacter = Pair.Value;
			FloorReturn = Pair.Key;
		}
	}

	return FloorReturn;
}

UFUNCTION()
void USN_DemonSubsystem::DemonDeath(int DeathExp)
{
	TArray<ASN_AbstractDemon*> aTmpDemons;

	for (ASN_AbstractDemon* Demon : aDemons)
	{
		if (!IsValid(Demon))
		{
			continue;
		}

		if (Demon->IsDeath() || Demon->IsExit())
		{
			//Demon->Destroy();
			continue;
		}

		aTmpDemons.Add(Demon);
	}

	aDemons = aTmpDemons;

	DemonDeathDelegate.Broadcast(DeathExp);
}

void USN_DemonSubsystem::AddNewGameLevel(int32 NewGameLevel)
{
	GameLevel = NewGameLevel;
}

TArray<FDemonGameSave> USN_DemonSubsystem::CreateDataDemonsToSave()
{
	TArray<FDemonGameSave> aSaveData;

	for (ASN_AbstractDemon* Demon : aDemons)
	{
		FDemonGameSave DataGameSave;
		DataGameSave.Location = Demon->GetActorLocation();
		DataGameSave.Rotation = Demon->GetActorRotation();
		DataGameSave.DemonClass = Demon->GetClass();

		aSaveData.Add(DataGameSave);
	}

	return aSaveData;
}

void USN_DemonSubsystem::ActiveSpawn()
{
	ActiveSpawner->Active();
	ActiveSpawner = nullptr;

	if (const UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::ReturnCameraToPlayer, 1.f, false);
	}
}

void USN_DemonSubsystem::ReturnCameraToPlayer()
{
	if (PlayerControllerRef)
	{
		PlayerControllerRef->SetViewTargetWithBlend(PlayerControllerRef->GetCharacter(), 3.f);
	}

	DemonsTurnEndDelegate.Broadcast();
}

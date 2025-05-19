// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SN_PlayerFP.h"
#include "EnhancedInputComponent.h"
#include "Actors/IMovement.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/SN_Objective.h"
#include "Camera/CameraComponent.h"
#include "Characters/SN_Character.h"
#include "Components/BillboardComponent.h"
#include "Core/SN_GameMode.h"
#include "Demons/DemonInterface.h"
#include "Demons/SN_DemonSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Library/SN_Library.h"

// Sets default values
ASN_PlayerFP::ASN_PlayerFP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("ROOT_COMPONENT");
	RootComponent = SceneComponent;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(GetRootComponent());

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");

	bActiveMovement = false;
	bEnableCameraMovement = true;

	ActionsRemaining = 3;
	bHasSearched = false;
	bHasHealing = false;
	bHasInvisiblity = false;
	bHasSpeed = false;

	bDisableMoveCameraToCharacter = false;
	bDamageAlways = false;
	bTeleportationActive = false;
	bInifiniteTurns = false;
	bInifiniteSearch = false;
	bInifiniteMagic = false;
	bRealizeAttackOnEndMovement = false;
}

void ASN_PlayerFP::SetupGameAndUIMode()
{
	if (IsValid(PlayerController))
	{
		FInputModeGameAndUI InputMode;

		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}

// Called when the game starts or when spawned
void ASN_PlayerFP::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->DestroyComponent();

	InitRefs();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASN_PlayerFP::InitDelegates, 2.0f, false);

	FTimerHandle TimerHandleMouse;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleMouse, this, &ASN_PlayerFP::ShowHealthWidgetDemons, 0.1f, true);
	
	//InitCharacterCards();
	//UpdateGameLevelHud();
}

void ASN_PlayerFP::InitDelegates()
{
	GMRef->FOnGameFinishedDelegate.AddDynamic(this, &ThisClass::RemoveHud);
	GMRef->CharacterExpUpDelegate.AddDynamic(this, &ThisClass::UpdateGameLevelHud);
	GMRef->DemonTurnInitDelegate.AddDynamic(this, &ThisClass::InitDemonTurn);
	GMRef->DemonTurnEndDelegate.AddDynamic(this, &ThisClass::EndDemonTurn);
	GMRef->InitCharacterTurnDelegate.AddUniqueDynamic(this, &ThisClass::InitCharacterValues);
	GMRef->GameLevelUpDelegate.AddUniqueDynamic(this, &ThisClass::ShowGameLevelUp);

	for (ASN_Character* Character : GMRef->GetCharacters())
	{
		Character->CharacterEndMovementDelegate.AddUniqueDynamic(this, &ThisClass::CharacterEndMovement);
		Character->CharacterEndActionDelegate.AddUniqueDynamic(this, &ThisClass::CharacterEndAction);
	}
}

void ASN_PlayerFP::CharacterEndMovement()
{
	CancelAction();
	ShowPlayerHud();
}

void ASN_PlayerFP::CharacterEndAction(bool bDecrementAction)
{
	if (bDecrementAction)
	{
		DecrementAction();
	}
	else
	{
		CancelAction();	
	}
		
	ShowPlayerHud();
}

void ASN_PlayerFP::ShowPlayerHud(bool bShowAll)
{
	BP_ShowPlayerHud(bShowAll);
}

void ASN_PlayerFP::Move(const FInputActionValue& Value)
{
	if (!bActiveMovement || !bEnableCameraMovement)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(CameraComponent->GetForwardVector(), MovementVector.Y);
	AddMovementInput(CameraComponent->GetRightVector(), MovementVector.X);
}

void ASN_PlayerFP::MoveUp(const FInputActionValue& Value)
{
	if (!bActiveMovement || !bEnableCameraMovement)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	AddMovementInput(CameraComponent->GetUpVector(), MovementVector.Y);
}

void ASN_PlayerFP::Look(const FInputActionValue& Value)
{
	if (!bActiveMovement || !bEnableCameraMovement)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	BP_Look(MovementVector.X, MovementVector.Y);

	//AddControllerPitchInput(MovementVector.Y);
	//AddControllerYawInput(MovementVector.X);
}

void ASN_PlayerFP::EnableMovement()
{
	bActiveMovement = true;
}

void ASN_PlayerFP::DisableMovement()
{
	bActiveMovement = false;
}

void ASN_PlayerFP::RemoveHud()
{
	BP_HidePlayerHud(false);
}

void ASN_PlayerFP::UpdateGameLevelHud()
{
	BP_UpdateGameLevelHud();
}

void ASN_PlayerFP::InitDemonTurn()
{
	BP_InitDemonTurn();
}

void ASN_PlayerFP::EndDemonTurn()
{
	if (GMRef->IsGameFinished())
	{
		return;
	}

	BP_EndDemonTurn();
}


void ASN_PlayerFP::ShowHealthWidgetDemons()
{
	FHitResult HitResult;

	if (PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel6, false, HitResult))
	{
		AActor* HitActor = HitResult.GetActor();

		if (!LastEnemyOverlap)
		{
			LastEnemyOverlap = HitActor;
			IDemonInterface* ShowActor = Cast<IDemonInterface>(HitActor);

			if (ShowActor)
			{
				ShowActor->EnableHealthWidget(true);
			}
		}
		else if (LastEnemyOverlap != HitActor)
		{
			IDemonInterface* OldShowActor = Cast<IDemonInterface>(LastEnemyOverlap);
			OldShowActor->EnableHealthWidget(false);

			LastEnemyOverlap = HitActor;
			IDemonInterface* ShowActor = Cast<IDemonInterface>(HitActor);

			if (ShowActor)
			{
				ShowActor->EnableHealthWidget(true);
			}
		}
	}
	else if (LastEnemyOverlap)
	{
		IDemonInterface* OldShowActor = Cast<IDemonInterface>(LastEnemyOverlap);
		OldShowActor->EnableHealthWidget(false);
		LastEnemyOverlap = nullptr;
	}
}

void ASN_PlayerFP::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//ShowHealthWidgetDemons();
}

void ASN_PlayerFP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerControllerInput = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerControllerInput->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this,
		                                   &ASN_PlayerFP::ClickActionFunction);

		EnhancedInputComponent->BindAction(FindCharacterAction, ETriggerEvent::Started, this,
		                                   &ASN_PlayerFP::MoveCameraToCharacter);

		EnhancedInputComponent->BindAction(ActiveMovementAction, ETriggerEvent::Started, this,
		                                   &ASN_PlayerFP::EnableMovement);
		EnhancedInputComponent->BindAction(ActiveMovementAction, ETriggerEvent::Completed, this,
		                                   &ASN_PlayerFP::DisableMovement);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASN_PlayerFP::Move);

		EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &ASN_PlayerFP::MoveUp);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASN_PlayerFP::Look);
	}
}

void ASN_PlayerFP::InitRefs()
{
	GMRef = Cast<ASN_GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

ASN_Character* ASN_PlayerFP::GetActiveCharacter() const
{
	if (!GMRef)
	{
		return nullptr;
	}

	return GMRef->GetActiveCharacter();
}

IIMovement* ASN_PlayerFP::GetFloorActiveCharacter()
{
	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return nullptr;
	}

	AActor* ActorFloor = USN_Library::GetActiveActorFloor(Character);

	if (!ActorFloor)
	{
		return nullptr;
	}

	if (IIMovement* ActiveFloor = Cast<IIMovement>(ActorFloor))
	{
		return ActiveFloor;
	}

	return nullptr;
}

void ASN_PlayerFP::InitMoveAction()
{
	Action = ETypeAction::Move;

	ASN_Character* Character = GetActiveCharacter();

	if (Character)
	{
		Character->ShowActiveFloors();
	}
}

bool ASN_PlayerFP::MoveToFloor()
{
	AActor* ActorClicked = GetActorClick(ECC_GameTraceChannel1);

	if (!ActorClicked)
	{
		return false;
	}

	IIMovement* Floor = Cast<IIMovement>(ActorClicked);

	if (bTeleportationActive)
	{
		FVector TargetLocation = GetLocationClick(ECC_GameTraceChannel1);

		GetActiveCharacter()->SetActorLocation(TargetLocation);
		return true;
	}

	if (Floor && (Floor->IsActiveMovement()))
	{
		ASN_Character* ActiveCharacter = GetActiveCharacter();

		if (!IsValid(ActiveCharacter))
		{
			return false;
		}

		ActiveCharacter->Jump();
		ActiveCharacter->PlayVFXStartDash();
		
		FVector TargetLocation = GetLocationClick(ECC_GameTraceChannel1);

		ActiveCharacter->SetActorLocation(TargetLocation);
		ActiveCharacter->PlayVFXEndDash();

		return true;
	}

	return false;
}

void ASN_PlayerFP::DecrementAction()
{
	if (!bInifiniteTurns)
	{
		ActionsRemaining--;
	}

	//Action = ETypeAction::None;
	GetActiveCharacter()->SetActionState(ETypeAction::None);

	BP_UpdateActionsInHud();

	CancelAction();
	
}

void ASN_PlayerFP::FinishTurn()
{
	GMRef->InitCharacterTurn();
}

void ASN_PlayerFP::InitCharacterValues()
{
	ASN_Character* Character = GetActiveCharacter();

	if (!IsValid(Character))
	{
		return;
	}

	ActionsRemaining = Character->GetNumActions();
	bHasSearched = false;
	bHasHealing = false;
	bHasSpeed = false;
	bHasInvisiblity = false;

	MoveCameraToCharacter(Character);

	BP_InitCharacterHud();
}

void ASN_PlayerFP::ShowGameLevelUp()
{
	ShowPlayerMessage(TEXT("El peligro aumenta… y la muerte acecha más cerca que nunca."));
}

void ASN_PlayerFP::MoveCameraToCharacter(ASN_Character* Character)
{
	if (!bEnableCameraMovement)
	{
		return;
	}

	bEnableCameraMovement = false;

	if (bDisableMoveCameraToCharacter)
	{
		bEnableCameraMovement = true;
		return;
	}

	UActorComponent* CharacterCameraLocation = Character->GetComponentByClass(UBillboardComponent::StaticClass());

	if (!CharacterCameraLocation)
	{
		return;
	}

	USceneComponent* CharacterSceneComponent = Cast<USceneComponent>(CharacterCameraLocation);

	if (!CharacterSceneComponent)
	{
		return;
	}

	SetActorLocation(CharacterSceneComponent->GetComponentLocation());

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Character->GetActorLocation());
	PlayerController->SetControlRotation(LookAtRotation);

	bEnableCameraMovement = true;
}

void ASN_PlayerFP::MoveCameraToCharacter()
{
	ASN_Character* Character = GetActiveCharacter();

	if (!IsValid(Character))
	{
		return;
	}

	MoveCameraToCharacter(Character);
}

void ASN_PlayerFP::InitCharacterCards()
{
	BP_ClearCardsInHud();

	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return;
	}

	if (Character->IsLeftItemInit())
	{
		BP_UpdateCardsInHud(true, Character->GetLeftItem());
	}

	if (Character->IsRightItemInit())
	{
		BP_UpdateCardsInHud(false, Character->GetRightItem());
	}

	if (Character->IsBodyItemInit())
	{
		BP_UpdateBodyCardInHud(Character->GetBodyItem());
	}
}

void ASN_PlayerFP::ClickActionFunction()
{
	switch (Action)
	{
	case ETypeAction::Move:
		{
			MovementAction();
			break;
		}
	case ETypeAction::Attack:
		{
			if (ActiveCardToAttack.bMagic)
			{
				MagicAction();
			}
			else
			{
				AttackAction();
			}

			break;
		}
	}
}

void ASN_PlayerFP::SearchAction()
{
	if (bHasSearched && !bInifiniteSearch)
	{
		ShowPlayerMessage(TEXT("You can only search once per turn."));
		return;
	}

	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return;
	}

	IIMovement* Floor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(Character));

	if (!Floor || (!Floor->CanSearch()))
	{
		ShowPlayerMessage(TEXT("Can't Search Outside"));
		return;
	}

	Action = ETypeAction::Search;
	Character->SetActionState(ETypeAction::Search);

	HidePlayerHud(false);

	Character->PlayVFXSearch();
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASN_PlayerFP::ShowSearchAction, 1.0f, false);
	
}

void ASN_PlayerFP::ShowSearchAction()
{

	ShowPlayerHud(false);
	
	FCardsItems CardItem = GMRef->GetRandomItem();

	if (CardItem.bZombie)
	{
		IIMovement* Floor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(GetActiveCharacter()));
		USN_DemonSubsystem* DemonSubsystem = UWorld::GetSubsystem<USN_DemonSubsystem>(GetWorld());
		DemonSubsystem->SpawnDemonsInFloor(Floor, true);
		DecrementAction();
	}
	else
	{
		
		BP_SearchAction(CardItem);
	}

	bHasSearched = true;
}

void ASN_PlayerFP::OpenDoorAction()
{
	Action = ETypeAction::OpenDoor;

	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return;
	}

	BP_HidePlayerHud(false);
	Character->OpenDoor();
}

void ASN_PlayerFP::CancelAction()
{

	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Cancel Action");
	
	bool bComeFromAttack = false;

	switch (Action)
	{
	case ETypeAction::Move:
		{
			GMRef->ResetAllFloors();
			break;
		}
	case ETypeAction::Attack:
		{
			bComeFromAttack = true;
			GMRef->ResetAllFloors();
			break;
		}
	}

	Action = ETypeAction::None;

	BP_CancelAction(bComeFromAttack);
}

void ASN_PlayerFP::MovementAction()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MovementAction");
	
	if (MoveToFloor())
	{
		DecrementAction();
		//CancelAction();
	}
}

void ASN_PlayerFP::InitAttackAction()
{
	ASN_Character* Character = GetActiveCharacter();

	if (!Character || (!Character->CanAttack()))
	{
		ShowPlayerMessage(TEXT("CANNOT ATTACK"));
		return;
	}

	Action = ETypeAction::Attack;

	bool bShowCards = true;

	if (Character->IsLeftItemInit() && !Character->IsRightItemInit())
	{
		ShowFloorsAttack(Character->GetLeftItem(), true);
		bShowCards = false;
	}

	if (!Character->IsLeftItemInit() && Character->IsRightItemInit())
	{
		ShowFloorsAttack(Character->GetRightItem(), false);
		bShowCards = false;
	}

	if (Character->GetLeftItem().bShield)
	{
		ShowFloorsAttack(Character->GetRightItem(), false);
		bShowCards = false;
	}

	if (Character->GetRightItem().bShield)
	{
		ShowFloorsAttack(Character->GetLeftItem(), true);
		bShowCards = false;
	}

	BP_HidePlayerHud(false);
	BP_InitAttackAction(Character->GetLeftItem(), Character->GetRightItem(), bShowCards);
}

void ASN_PlayerFP::ShowFloorsAttack(FCardsItems CardItem, bool bLeftCard)
{
	IIMovement* ActiveFloor = GetFloorActiveCharacter();

	if (!ActiveFloor)
	{
		return;
	}

	ActiveCardToAttack = CardItem;
	bCardToAttackIsLeft = bLeftCard;
	
	if (CardItem.bAttackDistance)
	{
		TArray<IIMovement*> FloorsToAttack;

		ActiveFloor->GetFloorsToAttack(FloorsToAttack, ActiveFloor, CardItem.Distance);

		for (IIMovement* Floor : FloorsToAttack)
		{
			if (!Floor || (Floor->IsClose()))
			{
				continue;
			}


			Floor->EnableDisableMovement(true);
		}
	}

	ActiveFloor->EnableDisableMovement(CardItem.bAttackMelee);

	BP_ShowFloorsAttack();
}

void ASN_PlayerFP::AttackAction()
{
	AActor* ActorClicked = GetActorClick(ECC_GameTraceChannel6);

	if (!ActorClicked)
	{
		return;
	}

	IIMovement* Floor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(ActorClicked));

	if (!Floor)
	{
		return;
	}

	if (!Floor->IsActiveMovement())
	{
		ShowPlayerMessage(TEXT("DISTANCE SO LONG"));
		return;
	}

	IHealthInterface* ActorToDamage = Cast<IHealthInterface>(ActorClicked);

	if (!ActorToDamage)
	{
		return;
	}

	RealizeAttack(Floor, ActorToDamage);
}

void ASN_PlayerFP::MagicAction()
{
	AActor* ActorClicked = GetActorClick(ECC_GameTraceChannel9);

	IHealthInterface* HealthClicked = Cast<IHealthInterface>(ActorClicked);

	if (!HealthClicked)
	{
		return;
	}

	IIMovement* Floor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(ActorClicked));

	if (!Floor)
	{
		return;
	}

	switch (ActiveCardToAttack.TypeMagic)
	{
	case ETypeMagic::Health:
		{
			if (bHasHealing && !bInifiniteMagic)
			{
				ShowPlayerMessage(TEXT("Solo una vez por turno"));
				CancelAction();
				return;
			}

			if (ASN_Character* Character = Cast<ASN_Character>(HealthClicked))
			{
				bHasHealing = true;
				HealthClicked->AddHealth(1);

				if (ActiveCardToAttack.SoundEffect)
				{
					UGameplayStatics::PlaySound2D(this, ActiveCardToAttack.SoundEffect, 1.0f, 1.0f, 0.0f);
				} 
	
				if (ActiveCardToAttack.EffectParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActiveCardToAttack.EffectParticles, ActorClicked->GetActorLocation(), ActorClicked->GetActorRotation(), true);
				}
				
			}
			else
			{
				return;
			}

			DecrementAction();
			
			break;
		}
	case ETypeMagic::Speed:
		{
			if (bHasSpeed && !bInifiniteMagic)
			{
				ShowPlayerMessage(TEXT("Solo una vez por turno"));
				CancelAction();
				return;
			}

			if (ActiveCardToAttack.SoundEffect)
			{
				UGameplayStatics::PlaySound2D(this, ActiveCardToAttack.SoundEffect, 1.0f, 1.0f, 0.0f);
			} 
	
			if (ActiveCardToAttack.EffectParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActiveCardToAttack.EffectParticles, ActorClicked->GetActorLocation(), ActorClicked->GetActorRotation(), true);
			}
			
			DecrementAction();
			
			bHasSpeed = true;;
			break;
		}
	case ETypeMagic::Invisiblity:
		{
			if (bHasInvisiblity && !bInifiniteMagic)
			{
				ShowPlayerMessage(TEXT("Solo una vez por turno"));
				CancelAction();
				return;
			}

			bHasInvisiblity = true;
			HealthClicked->SetInvisible(true);

			if (ActiveCardToAttack.SoundEffect)
			{
				UGameplayStatics::PlaySound2D(this, ActiveCardToAttack.SoundEffect, 1.0f, 1.0f, 0.0f);
			} 
	
			if (ActiveCardToAttack.EffectParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActiveCardToAttack.EffectParticles, ActorClicked->GetActorLocation(), ActorClicked->GetActorRotation(), true);
			}
			
			DecrementAction();
			
			break;
		}
	case ETypeMagic::Attack:
		{

			ASN_Character* Character = Cast<ASN_Character>(HealthClicked);
			
			if (!IsValid(Character))
			{
				GetActiveCharacter()->RealizeAttack(ActiveCardToAttack, Floor, HealthClicked, bCardToAttackIsLeft);
			}
			else
			{
				return;
			}
			
			break;
		}
	}
	

	
	
	CancelAction();
}

void ASN_PlayerFP::ShowPlayerMessage(const FString& Message)
{
	BP_ShowPlayerMessageInHud(Message);
}

void ASN_PlayerFP::RealizeAttack(IIMovement* FloorTarget, IHealthInterface* ActorToDamage)
{
	if (bDamageAlways)
	{
		GetActiveCharacter()->SetInvisible(false);
		ActorToDamage->RecieveDamage(ActiveCardToAttack.Damage);
		return;
	}

	GetActiveCharacter()->RealizeAttack(ActiveCardToAttack, FloorTarget, ActorToDamage, bCardToAttackIsLeft);
}

FVector ASN_PlayerFP::GetLocationClick(const ECollisionChannel CollisionChannel) const
{
	if (!PlayerController)
	{
		return FVector::ZeroVector;
	}

	float MouseX, MouseY;

	PlayerController->GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;

	if (PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
	{
		FVector EndTargetLocation = WorldDirection * 100000000.0;
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		bool bHit = this->GetWorld()->LineTraceSingleByChannel(
			HitResult,
			WorldLocation,
			EndTargetLocation,
			CollisionChannel,
			TraceParams
		);

		if (!bHit)
		{
			return FVector::ZeroVector;
		}

		return HitResult.Location;
	}

	return FVector::ZeroVector;
}

AActor* ASN_PlayerFP::GetActorClick(ECollisionChannel CollisionChannel) const
{
	if (!PlayerController)
	{
		return nullptr;
	}

	float MouseX, MouseY;

	PlayerController->GetMousePosition(MouseX, MouseY);

	FVector WorldLocation, WorldDirection;

	if (PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection))
	{
		FVector EndTargetLocation = WorldDirection * 100000000.0;

		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		bool bHit = this->GetWorld()->LineTraceSingleByChannel(
			HitResult,
			WorldLocation,
			EndTargetLocation,
			CollisionChannel,
			TraceParams
		);

		if (!bHit)
		{
			return nullptr;
		}

		return HitResult.GetActor();
	}

	return nullptr;
}

void ASN_PlayerFP::TakeObjective()
{
	ASN_Character* Character = GetActiveCharacter();

	if (!Character)
	{
		return;
	}

	AActor* ActiveActor = USN_Library::GetActiveActorFloor(Character);

	IIMovement* Floor = Cast<IIMovement>(ActiveActor);

	if (!Floor)
	{
		return;
	}

	AActor* ActorObjective = Floor->GetObjectiveActor();

	ASN_Objective* Objective = Cast<ASN_Objective>(ActorObjective);

	if (!Objective || (!Objective->IsActive()))
	{
		ShowPlayerMessage(TEXT("There is no goal!"));
		return;
	}

	if (Objective->Take(Character))
	{
		ShowPlayerMessage("Goal achieved!");
	}
	else
	{
		ShowPlayerMessage("Something's not right... This doesn't seem to be what you were looking for.");
	}

	DecrementAction();
}


void ASN_PlayerFP::HidePlayerHud(bool bHideAll)
{
	BP_HidePlayerHud(bHideAll);
}

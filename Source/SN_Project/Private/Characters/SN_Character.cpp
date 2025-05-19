// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SN_Character.h"

#include "AIController.h"
#include "Actors/IMovement.h"
#include "Characters/SN_CharacterAnimInstance.h"
#include "SN_Components/SN_HealtComponent.h"
#include "Components//BillboardComponent.h"
#include "Hud/SN_WCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Library/SN_Library.h"
#include "Particles/ParticleSystemComponent.h"

class AAIController;
// Sets default values
ASN_Character::ASN_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SMPosition = CreateDefaultSubobject<UStaticMeshComponent>("SMPosition");
	SMPosition->SetupAttachment(GetRootComponent());
	SMPosition->SetHiddenInGame(true);

	LeftWeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>("LeftWeaponComponent");
	LeftWeaponComponent->SetupAttachment(GetRootComponent());

	RightWeaponComponent = CreateDefaultSubobject<UStaticMeshComponent>("RightWeaponComponent");
	RightWeaponComponent->SetupAttachment(GetRootComponent());

	LeftParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>("LeftParticleComponent");
	LeftParticleComponent->SetupAttachment(GetRootComponent());

	RightParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>("RightParticleComponent");
	RightParticleComponent->SetupAttachment(GetRootComponent());

	CameraLocation = CreateDefaultSubobject<UBillboardComponent>("CameraLocation");
	CameraLocation->SetupAttachment(GetRootComponent());

	EnemyAttackPositionComponent1 = CreateDefaultSubobject<UBillboardComponent>("EnemyAttackPositionComponent1");
	EnemyAttackPositionComponent1->SetupAttachment(GetRootComponent());

	EnemyAttackPositionComponent2 = CreateDefaultSubobject<UBillboardComponent>("EnemyAttackPositionComponent2");
	EnemyAttackPositionComponent2->SetupAttachment(GetRootComponent());

	EnemyAttackPositionComponent3 = CreateDefaultSubobject<UBillboardComponent>("EnemyAttackPositionComponent3");
	EnemyAttackPositionComponent3->SetupAttachment(GetRootComponent());

	EnemyAttackPositionComponent4 = CreateDefaultSubobject<UBillboardComponent>("EnemyAttackPositionComponent4");
	EnemyAttackPositionComponent4->SetupAttachment(GetRootComponent());

	HealthComponent = CreateDefaultSubobject<USN_HealtComponent>("HealthComponent");

	NumActions = 3;
	ExpCharacter = 0;
	LevelCharacter = 1;
	CountEnemiesAttackMe = 0;

	bLeftItemInit = false;
	bRightItemInit = false;
	bBodyItemInit = false;
	bEscape = false;
	bImmortal = false;
	bInvisible = false;
	bNeverOpenDoor = false;

	MoveRadiusToDemon = 50.f;

	ActionState = ETypeAction::None;

	aEnemyAttackPositionFree.Init(true, 4);
}

// Called when the game starts or when spawned
void ASN_Character::BeginPlay()
{
	Super::BeginPlay();

	CharacterController = Cast<AAIController>(GetController());

	if (IsValid(CharacterController))
	{
		CharacterController->ReceiveMoveCompleted.AddDynamic(this, &ASN_Character::OnMoveCompleted);
	}

	AnimInstance = Cast<USN_CharacterAnimInstance>(GetMesh()->GetAnimInstance());

	HealthComponent->DeathDelegate.AddDynamic(this, &ThisClass::Death);
	HealthComponent->UpdateHealthDelegate.AddDynamic(this, &ThisClass::UpdateHudHealth);
	UpdateHudHealth();
}

void ASN_Character::UpdateHudHealth()
{
	BP_UpdateHudHealth();
}

bool ASN_Character::SaveDamage()
{
	if (bBodyItemInit)
	{
		int RandomNumber = FMath::RandRange(0, 6);

		if (RandomNumber >= BodyItem.Armor)
		{
			USN_Library::ShowMessageToPlayer(GetWorld(), "Protect By Armor");
			return true;
		}
	}

	if (bLeftItemInit && (LeftItem.bShield))
	{
		int RandomNumber = FMath::RandRange(0, 6);

		if (RandomNumber >= LeftItem.Armor)
		{
			USN_Library::ShowMessageToPlayer(GetWorld(), "Protect By Shield");
			return true;
		}
	}

	if (bRightItemInit && (RightItem.bShield))
	{
		int RandomNumber = FMath::RandRange(0, 6);

		if (RandomNumber >= RightItem.Armor)
		{
			USN_Library::ShowMessageToPlayer(GetWorld(), "Protect By Shield");
			return true;
		}
	}

	return false;
}

void ASN_Character::CheckCharacterLevel()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CharacterLevel");

	if (LevelCharacter == 4)
	{
		return;
	}

	bool bUpLevel = false;


	switch (LevelCharacter)
	{
	case 1:
		{
			if (ExpCharacter >= 7)
			{
				bUpLevel = true;
			}

			break;
		}
	case 2:
		{
			if (ExpCharacter >= 19)
			{
				bUpLevel = true;
			}

			break;
		}
	case 3:
		{
			if (ExpCharacter >= 43)
			{
				bUpLevel = true;
			}

			break;
		}
	}

	if (bUpLevel)
	{
		++LevelCharacter;
		++NumActions;
		CharacterLevelUpDelegate.Broadcast();
	}
}

void ASN_Character::RotateToActorToDamage()
{
	if (!ActorToDamageInAttack)
	{
		return;
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(), ActorToDamageInAttack->GetLocation());
	//SetActorRotation(LookAtRotation);
	SetActorRotation(FRotator(GetActorRotation().Pitch, LookAtRotation.Yaw, LookAtRotation.Roll));
}

void ASN_Character::RotateToDoor()
{
	IIMovement* FloorActive = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this));

	if (!FloorActive)
	{
		return;
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(), FloorActive->GetDoorLocation());
	SetActorRotation(FRotator(GetActorRotation().Pitch, LookAtRotation.Yaw, LookAtRotation.Roll));
}


void ASN_Character::EnableDisableShowPosition(bool bEnabled)
{
	SMPosition->SetHiddenInGame(!bEnabled);
}

void ASN_Character::Death()
{
	USN_Library::ShowMessageToPlayer(GetWorld(), "A hero has fallen into darkness... but their courage will endure.");

	FOnCharacterDeathDelegate.Broadcast();

	if (DeathAnimation)
	{
		GetMesh()->PlayAnimation(DeathAnimation, false);
	}

	BP_Death();
	
}

void ASN_Character::ShowActiveFloors()
{	
	if (IIMovement* Floor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this)))
	{
		Floor->ShowActiveFloorsToMovement();
	}
}

void ASN_Character::MoveToFloor(AActor* ActorRef)
{
	if (!ActorRef)
	{
		return;
	}

	IIMovement* FloorRef = Cast<IIMovement>(ActorRef);

	if (!FloorRef)
	{
		return;
	}

	CharacterController->MoveToLocation(FloorRef->GetRandomLocationInFloor());
}

void ASN_Character::MoveToLocation(FVector TargetLocation, float Radius)
{
	CharacterController->MoveToLocation(TargetLocation, Radius);
}

void ASN_Character::AddItem(bool bLeft, FCardsItems NewItem)
{
	if (bLeft)
	{
		LeftItem = NewItem;
		bLeftItemInit = true;
		AttachWeaponCard(GetLeftItem(), true);
	}
	else
	{
		RightItem = NewItem;
		bRightItemInit = true;
		AttachWeaponCard(GetRightItem(), false);
	}
}

void ASN_Character::AddBodyItem(FCardsItems NewItem)
{
	BodyItem = NewItem;
	bBodyItemInit = true;
}

bool ASN_Character::CanOpenDoors()
{
	return LeftItem.bDoor || RightItem.bDoor;
}

int ASN_Character::GetMinValueItemOpenDoors()
{
	if (bLeftItemInit && LeftItem.bDoor && bRightItemInit && RightItem.bDoor)
	{
		return LeftItem.DiceDoor < RightItem.DiceDoor ? LeftItem.DiceDoor : RightItem.DiceDoor;;
	}

	if (bLeftItemInit && LeftItem.bDoor)
	{
		return LeftItem.DiceDoor;
	}
	else
	{
		return RightItem.DiceDoor;
	}
}

bool ASN_Character::CanAttack()
{
	if (LeftItem.bAttackMelee || LeftItem.bAttackDistance || RightItem.bAttackMelee || RightItem.bAttackDistance)
	{
		return true;
	}

	return false;
}

void ASN_Character::ClearAllItems()
{
	bBodyItemInit = false;
	bLeftItemInit = false;
	bRightItemInit = false;

	LeftItem = FCardsItems();
	RightItem = FCardsItems();
	BodyItem = FCardsItems();
}

void ASN_Character::RecieveDamage(int Damage)
{
	if (bImmortal)
	{
		return;
	}

	if (!SaveDamage())
	{
		HealthComponent->RecieveDamage(Damage);
	}
}

bool ASN_Character::IsDeath() const
{
	return HealthComponent->IsDeath();
}

int ASN_Character::GetMinimunForceToDamage() const
{
	return HealthComponent->GetMinimunForceToDamage();
}

void ASN_Character::AddHealth(int NewHealth)
{
	HealthComponent->AddHealth(NewHealth);
}

void ASN_Character::SetHealth(int NewHealth)
{
	HealthComponent->SetHealth(NewHealth);
}

int ASN_Character::GetHealth() const 
{
	return HealthComponent->GetCurrentHealth();
}

void ASN_Character::AddExp(int NewExp)
{
	ExpCharacter = ExpCharacter + NewExp;
	WCharacterRef->BP_UpdateExp(ExpCharacter);
	CheckCharacterLevel();
}

int ASN_Character::GetNumActions()
{
	return NumActions;
}

void ASN_Character::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	switch (ActionState)
	{
	case ETypeAction::OpenDoor:
		{
			RotateToDoor();

			if (LeftItem.bDoor)
			{
				RealizeAnimation(LeftItem.AnimationType, true);
			}
			else
			{
				RealizeAnimation(RightItem.AnimationType, false);
			}

			break;
		}
	case ETypeAction::Attack:
		{
			RotateToActorToDamage();
			RealizeAnimation(CardUseInAttack.AnimationType, bCardToAttackIsLeft);
			break;
		}
	default:
		{
			CharacterEndMovementDelegate.Broadcast();
			break;
		}
	}
}

FVector ASN_Character::GetLocation() const
{
	return GetActorLocation();
}

void ASN_Character::CheckMeleeWeaponImpact()
{
	switch (ActionState)
	{
	case ETypeAction::OpenDoor:
		{
			CheckOpenDoor();
			CharacterEndActionDelegate.Broadcast(true);
			break;
		}
	case ETypeAction::Attack:
		{
			RealizeDamage(CardUseInAttack, FloorTargetAttack, ActorToDamageInAttack);
			break;
		}
	}

	ActionState = ETypeAction::None;
	
}

void ASN_Character::AttachWeaponCard(FCardsItems Item, bool bHandLeft)
{
	if (Item.bMagic && !Item.Particles)
	{
		return;
	}
	else if (!Item.bMagic && !Item.Mesh)
	{
		return;
	}

	// Referencias a componentes según la mano
	UStaticMeshComponent* WeaponComponent = bHandLeft ? LeftWeaponComponent : RightWeaponComponent;
	UParticleSystemComponent* ParticleComponent = bHandLeft ? LeftParticleComponent : RightParticleComponent;

	// Detach ambos componentes antes de asignar el nuevo
	if (WeaponComponent && WeaponComponent->IsRegistered())
	{
		WeaponComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		WeaponComponent->SetStaticMesh(nullptr); // Limpia el mesh anterior
	}

	if (ParticleComponent && ParticleComponent->IsRegistered())
	{
		ParticleComponent->DeactivateSystem(); // Desactiva el sistema anterior
		ParticleComponent->SetTemplate(nullptr);
		ParticleComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	// Si es mágico, usar sistema de partículas
	if (Item.bMagic && Item.Particles)
	{
		UParticleSystemComponent* TargetComponent = bHandLeft ? LeftParticleComponent : RightParticleComponent;
		TargetComponent->SetTemplate(Item.Particles);
		TargetComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                   bHandLeft
			                                   ? FName(*(Item.SocketName.ToString() + "_L"))
			                                   : FName(*(Item.SocketName.ToString() + "_R")));
		TargetComponent->bAutoActivate = true;
		TargetComponent->ActivateSystem(true);
	}
	else if (Item.Mesh) // Si es un arma física
	{
		UStaticMeshComponent* TargetComponent = bHandLeft ? LeftWeaponComponent : RightWeaponComponent;
		TargetComponent->SetStaticMesh(Item.Mesh);
		TargetComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                   bHandLeft
			                                   ? FName(*(Item.SocketName.ToString() + "_L"))
			                                   : FName(*(Item.SocketName.ToString() + "_R")));
		TargetComponent->RegisterComponent();
	}
}


void ASN_Character::PlayVFXStartDash()
{
	if (TeleportSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, TeleportSoundCue, 1.0f, 1.0f, 0.0f);
	}

	if (StartDashParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), StartDashParticles, GetActorLocation(), GetActorRotation(),
		                                         true);
	}
}

void ASN_Character::PlayVFXEndDash()
{
	if (EndDashParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndDashParticles, GetActorLocation(), GetActorRotation(),
		                                         FVector(0.2, 0.2, 0.2), true);
	}
}

void ASN_Character::PlayVFXSearch()
{
	if (SearchSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, SearchSoundCue, 1.0f, 1.0f, 0.0f);
	}
}

void ASN_Character::ClearCountEnemiesAttackMe()
{
	aEnemyAttackPositionFree[0] = true;
	aEnemyAttackPositionFree[1] = true;
	aEnemyAttackPositionFree[2] = true;
	aEnemyAttackPositionFree[3] = true;
}

void ASN_Character::AddCountEnemiesAttackMe()
{
	++CountEnemiesAttackMe;
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("AddCountEnemy"));
}

FVector ASN_Character::GetFreePostionToAttack()
{
	int IndexFree = -1;

	for (int i = 0; i < aEnemyAttackPositionFree.Num(); i++)
	{
		if (aEnemyAttackPositionFree[i])
		{
			aEnemyAttackPositionFree[i] = false;
			IndexFree = i;
			break;
		}
	}

	switch (IndexFree)
	{
	case 0:
		{
			return EnemyAttackPositionComponent1->GetComponentLocation();
		}
	case 1:
		{
			return EnemyAttackPositionComponent2->GetComponentLocation();
		}
	case 2:
		{
			return EnemyAttackPositionComponent3->GetComponentLocation();
		}
	case 3:
		{
			return EnemyAttackPositionComponent4->GetComponentLocation();
		}
	}

	return FVector::ZeroVector;
}

void ASN_Character::OpenDoor()
{
	IIMovement* FloorActive = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this));

	if (!FloorActive)
	{
		CharacterEndActionDelegate.Broadcast(false);
		return;
	}

	if (!FloorActive->ExistDoor())
	{		
		USN_Library::ShowMessageToPlayer(GetWorld(), "THERE IS NO DOOR!");
		CharacterEndActionDelegate.Broadcast(false);
		return;
	}

	if (!CanOpenDoors())
	{
		USN_Library::ShowMessageToPlayer(GetWorld(), "YOU CAN'T OPEN DOORS!");
		CharacterEndActionDelegate.Broadcast(false);
		return;
	}

	ActionState = ETypeAction::OpenDoor;
	MoveToLocation(FloorActive->GetLocationOpenDoor(), 40.f);
}

void ASN_Character::RealizeAttack(FCardsItems CardToAttack, IIMovement* FloorTarget, IHealthInterface* ActorToDamage,
                                  bool bLeftCard)
{
	if (ActorToDamage->GetMinimunForceToDamage() > CardToAttack.Damage)
	{
		USN_Library::ShowMessageToPlayer(GetWorld(),TEXT("The weapon is not strong enough"));
		return;
	}

	ActionState = ETypeAction::Attack;

	SetInvisible(false);

	if (CardToAttack.bRequiereMovement)
	{
		CardUseInAttack = CardToAttack;
		bCardToAttackIsLeft = bLeftCard;
		ActorToDamageInAttack = ActorToDamage;
		FloorTargetAttack = FloorTarget;
		MoveToLocation(ActorToDamage->GetLocation(), MoveRadiusToDemon);
	}
	else
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), ActorToDamage->GetLocation());
		SetActorRotation(FRotator(GetActorRotation().Pitch, LookAtRotation.Yaw, LookAtRotation.Roll));
		
		RealizeAnimation(CardToAttack.AnimationType, bLeftCard);		
		//RealizeDamage(CardToAttack, FloorTarget, ActorToDamage);
		
		BP_LaunchMagic(CardToAttack, Cast<AActor>(FloorTarget), Cast<AActor>(ActorToDamage), bLeftCard);
		
		ActionState = ETypeAction::None;		
		//CharacterEndActionDelegate.Broadcast(true);
	}
}

void ASN_Character::RealizeDamageReturnBP(const FCardsItems& CardToAttack, AActor* FloorTarget,
	AActor* ActorToDamage)
{
	RealizeDamage(CardToAttack, Cast<IIMovement>(FloorTarget), Cast<IHealthInterface>(ActorToDamage));
}

void ASN_Character::RealizeDamage(const FCardsItems& CardToAttack, IIMovement* FloorTarget,
                                  IHealthInterface* ActorToDamage)
{
	bool bFaliedAttack = true;

	IIMovement* ActiveFloor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this));

	for (int i = 0; i < CardToAttack.Dice; ++i)
	{
		int Dice = USN_Library::GetRandomDiceNumber();

		if (Dice < CardToAttack.Hit && Dice == 1)
		{
			if (ActiveFloor && (ActiveFloor != FloorTarget))
			{
				if (!RealizeFriendDamage(CardToAttack, FloorTarget->GetOverlappingCharacters()))
				{
					bFaliedAttack = false;
				}
			}
		}
		else if (Dice >= CardToAttack.Hit)
		{
			ActorToDamage->RecieveDamage(CardToAttack.Damage);
			bFaliedAttack = false;
		}
	}

	if (bFaliedAttack)
	{
		USN_Library::ShowMessageToPlayer(GetWorld(), TEXT("Attack failed!"));
	}

	CharacterEndActionDelegate.Broadcast(true); 
}

void ASN_Character::CheckOpenDoor()
{
	if (bNeverOpenDoor)
	{
		USN_Library::ShowMessageToPlayer(GetWorld(), TEXT("You couldn't open the door!"));
		return;
	}

	IIMovement* FloorActive = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this));

	FloorActive->PlayVFXHitDoor();

	int RandomValue = USN_Library::GetRandomDiceNumber();

	if (RandomValue < GetMinValueItemOpenDoors())
	{
		USN_Library::ShowMessageToPlayer(GetWorld(), TEXT("You couldn't open the door!"));
		return;
	}

	FloorActive->OpenDoor();
}

void ASN_Character::RealizeAnimation(EAnimationType AnimationType, bool bLeft)
{
	if (bLeft)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Realizing LEFT animation"));
		switch (AnimationType)
		{
		case EAnimationType::Light:
			{
				AnimInstance->PlayLightMontageLeft();
				break;
			}
		case EAnimationType::Heavy:
			{
				AnimInstance->PlayHeavyMontageLeft();
				break;
			}
		case EAnimationType::Magic:
			{
				AnimInstance->PlayMagicMontageLeft();
				break;
			}
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Realizing RIGHT animation"));
		switch (AnimationType)
		{
		case EAnimationType::Light:
			{
				AnimInstance->PlayLightMontageRight();
				break;
			}
		case EAnimationType::Heavy:
			{
				AnimInstance->PlayHeavyMontageRight();
				break;
			}
		case EAnimationType::Magic:
			{
				AnimInstance->PlayMagicMontageRight();
				break;
			}
		}
	}
}

bool ASN_Character::RealizeFriendDamage(FCardsItems CardToAttack, TArray<AActor*> DamageActors)
{
	if (DamageActors.IsEmpty())
	{
		return false;
	}

	TArray<IHealthInterface*> LifeActors;

	for (AActor* Actor : DamageActors)
	{
		IHealthInterface* ActorHealthComponent = Cast<IHealthInterface>(Actor);

		if (!ActorHealthComponent || (ActorHealthComponent->IsDeath()))
		{
			continue;
		}

		LifeActors.Add(ActorHealthComponent);
	}

	if (LifeActors.IsEmpty())
	{
		return false;
	}

	int RandomNumber = FMath::RandRange(0, LifeActors.Num() - 1);

	if (RandomNumber < 0 || RandomNumber >= LifeActors.Num())
	{
		return false;
	}

	IHealthInterface* DamageActor = LifeActors[RandomNumber];

	if (!DamageActor)
	{
		return false;
	}

	if (this == DamageActor)
	{
		return false;
	}

	USN_Library::ShowMessageToPlayer(GetWorld(),TEXT("Daño amigo!"));

	DamageActor->RecieveDamage(CardToAttack.Damage);

	return true;
}

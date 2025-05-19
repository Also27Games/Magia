// Fill out your copyright notice in the Description page of Project Settings.


#include "Demons/SN_AbstractDemon.h"

#include "Actors/IMovement.h"
#include "DrawDebugHelpers.h"
#include "Library/SN_Library.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Demons/SN_DemonAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "SN_Components/HealthInterface.h"
#include "SN_Components/SN_HealtComponent.h"

// Sets default values
ASN_AbstractDemon::ASN_AbstractDemon()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	HealthComponent = CreateDefaultSubobject<USN_HealtComponent>("HealthComponent");

	DamageValue = 1;
	NumActions = 1;
	DeathExp = 1;
	bActive = true;
	bActiveSpawnAnimation = true;

	bInitEndTurnDelegate = false;
	bIsExit = false;

	bDrawLineTraceEnemyEye = false;

	ActionState = EDemonAction::None;

	MoveRadiusToCharacter = 40.f;
}

// Called when the game starts or when spawned
void ASN_AbstractDemon::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->DeathDelegate.AddDynamic(this, &ThisClass::Death);
	HealthComponent->UpdateHealthDelegate.AddDynamic(this, &ThisClass::UpdateHudHealth);
	UpdateHudHealth();

	DemonController = Cast<AAIController>(GetController());

	if (IsValid(DemonController))
	{
		DemonController->ReceiveMoveCompleted.AddDynamic(this, &ASN_AbstractDemon::OnMoveCompleted);
	}

	if (!bActive)
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		bActiveSpawnAnimation = false;
	}

	AnimInstance = Cast<USN_DemonAnimInstance>(GetMesh()->GetAnimInstance());

	GetCharacterMovement()->SetRVOAvoidanceWeight(FMath::RandRange(0, 1));
}

void ASN_AbstractDemon::UpdateHudHealth()
{
	BP_UpdateHudHealth();
}

void ASN_AbstractDemon::DestroyDemon()
{
	Destroy();
}

void ASN_AbstractDemon::Death()
{
	if (DeathParticles)
	{
		SetActorHiddenInGame(true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticles, GetActorLocation(), GetActorRotation(),
		                                         true);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ASN_AbstractDemon::DestroyDemon, 3.0f, false);

	DeathDemonDelegate.Broadcast(DeathExp);
}

void ASN_AbstractDemon::InicializeDemonValues()
{
	if (HealthComponent->IsDeath())
	{
		return;
	}

	NumActionsRemaing = NumActions;
}

void ASN_AbstractDemon::RealizeAction()
{
	ActionState = EDemonAction::None;

	if (NumActionsRemaing == 0)
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	AActor* ActiveActor = USN_Library::GetActiveActorFloor(this);

	if (!ActiveActor)
	{
		return;
	}

	--NumActionsRemaing;

	if (ExistCharacterToAttack())
	{
		Attack();
	}
	else
	{
		Movement(TargetFloorAction, aCharactersAction);
	}
}

void ASN_AbstractDemon::PlaySpawnMontage()
{
	if (SpawnMontage)
	{
		AnimInstance->PlayMontage(SpawnMontage);
	}
}

void ASN_AbstractDemon::PlaySpawnParticles()
{
	if (SpawnParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpawnParticles, GetActorLocation(), GetActorRotation(),
		                                         true);
	}
}

FVector ASN_AbstractDemon::GetFreePostionToAttack()
{
	return FVector(0, 0, 0);
}

bool ASN_AbstractDemon::ExistCharacterToAttack()
{
	IIMovement* ActiveFloor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this));

	if (!ActiveFloor)
	{
		return false;
	}

	TArray<AActor*> aActorsOverlap = ActiveFloor->GetOverlappingCharacters();

	if (aActorsOverlap.IsEmpty())
	{
		return false;
	}

	bool bExistCharacter = false;

	for (AActor* Actor : aActorsOverlap)
	{
		IHealthInterface* HealthInterface = Cast<IHealthInterface>(Actor);

		if (!HealthInterface)
		{
			continue;
		}

		if (!HealthInterface->IsDeath())
		{
			bExistCharacter = true;
			break;
		}
	}

	return bExistCharacter;
}

void ASN_AbstractDemon::Attack()
{
	IIMovement* ActiveFloor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this));

	if (!ActiveFloor)
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	TArray<AActor*> aActorsOverlap = ActiveFloor->GetOverlappingCharacters();

	if (aActorsOverlap.IsEmpty())
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	TArray<AActor*> LifeActors;

	for (int i = 0; i < aActorsOverlap.Num(); i++)
	{
		AActor* TmpActor = aActorsOverlap[i];

		if (!TmpActor)
		{
			continue;
		}

		IHealthInterface* TmpHealth = Cast<IHealthInterface>(TmpActor);

		if (!TmpHealth || (TmpHealth->IsDeath()))
		{
			continue;
		}

		LifeActors.Add(TmpActor);
	}

	if (LifeActors.IsEmpty())
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	const int Max = LifeActors.Num() - 1;

	int RandomNumber = FMath::RandRange(0, Max);

	AActor* TmpActor = LifeActors[RandomNumber];

	if (!TmpActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "ACTOR DAMAGE NULL!!");
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	IHealthInterface* TmpHealth = Cast<IHealthInterface>(TmpActor);

	FVector TargetLocation = TmpHealth->GetLocation();

	if (TargetLocation.IsZero())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "TARGET ZERO!!");
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	ActionState = EDemonAction::Attack;
	ActorToDamage = TmpActor;

	DemonController->MoveToLocation(TargetLocation, MoveRadiusToCharacter);
}

void ASN_AbstractDemon::Movement(AActor* TargetActor, const TArray<AActor*>& aCharacters,
                                 bool bCheckCharacters)
{
	ActionState = EDemonAction::Move;

	IIMovement* ActiveFloor = Cast<IIMovement>(USN_Library::GetActiveActorFloor(this));

	if (!ActiveFloor)
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	if (bCheckCharacters)
	{
		IIMovement* VisibleFloor = GetFloorVisibleCharacter(ActiveFloor, aCharacters);

		if (VisibleFloor)
		{
			TargetActor = Cast<AActor>(VisibleFloor);
		}
	}

	if (!IsValid(TargetActor))
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	if (IIMovement* DestinyFloor = GetFloorByNormalMovement(ActiveFloor, TargetActor))
	{
		if (!IsValid(DemonController))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "DEMON CONTROLLER NULL!!!!");
			return;
		}

		DemonController->MoveToLocation(DestinyFloor->GetRandomLocationInFloor());
	}
}

IIMovement* ASN_AbstractDemon::GetFloorByNormalMovement(IIMovement* ActiveFloor, AActor* TargetActor)
{
	TArray<AActor*> aOverlappingFloors = ActiveFloor->GetOverlappingFloors();

	float NearDistance = 9999999999.0f;

	IIMovement* DestinyFloor = nullptr;

	for (AActor* TmpActor : aOverlappingFloors)
	{
		IIMovement* TmpFloor = Cast<IIMovement>(TmpActor);

		if (TmpFloor->IsClose())
		{
			continue;
		}

		FVector TmpLocation = TmpFloor->GetActorLocation();

		float Distance = FVector::Dist(TmpLocation, TargetActor->GetActorLocation());

		if (Distance < NearDistance)
		{
			NearDistance = Distance;
			DestinyFloor = TmpFloor;
		}
	}

	return DestinyFloor;
}

IIMovement* ASN_AbstractDemon::GetFloorVisibleCharacter(IIMovement* ActiveFloor, TArray<AActor*> aCharacters)
{
	IIMovement* DestinyFloor = nullptr;

	TMap<FString, AActor*> mapVisibleFloors;

	for (AActor* Actor : aCharacters)
	{
		IHealthInterface* TmpHealth = Cast<IHealthInterface>(Actor);

		if (!TmpHealth || (TmpHealth->IsDeath()))
		{
			continue;
		}

		FHitResult HitResult;
		FCollisionQueryParams TraceParams;

		bool bHit = this->GetWorld()->LineTraceSingleByChannel(
			HitResult,
			GetActorLocation(),
			Actor->GetActorLocation(),
			ECC_GameTraceChannel4,
			TraceParams
		);

		/*if (bDrawLineTraceEnemyEye)
		{
			FColor LineColor = bHit ? FColor::Green : FColor::Red;

			DrawDebugLine(
				GetWorld(),
				GetActorLocation(),
				Actor->GetActorLocation(),
				LineColor,
				false, // No permanece en el mundo para siempre
				10.0f, // Duración de la línea en segundos
				0,
				3.0f // Grosor de la línea
			);
		}*/

		if (!bHit)
		{
			continue;
		}

		IHealthInterface* HealthActor = Cast<IHealthInterface>(HitResult.GetActor());

		if (!HealthActor)
		{
			continue;
		}

		if (HealthActor->IsInvisible())
		{
			continue;
		}

		AActor* ActorFloor = USN_Library::GetActiveActorFloor(HitResult.GetActor());

		if (!ActorFloor)
		{
			continue;
		}

		if (!mapVisibleFloors.Contains(ActorFloor->GetName()))
		{
			mapVisibleFloors.Add(ActorFloor->GetName(), ActorFloor);
		}
	}

	TArray<AActor*> aVisibleFloors;

	mapVisibleFloors.GenerateValueArray(aVisibleFloors);

	if (aVisibleFloors.IsEmpty())
	{
		return nullptr;
	}

	if (aVisibleFloors.Num() == 1)
	{
		DestinyFloor = Cast<IIMovement>(aVisibleFloors[0]);

		if (!DestinyFloor)
		{
			return nullptr;
		}

		if (DestinyFloor->IsClose())
		{
			return nullptr;
		}
	}
	else
	{
		float NearDistance = 9999999999.0f;

		for (AActor* TmpActor : aVisibleFloors)
		{
			IIMovement* TmpFloor = Cast<IIMovement>(TmpActor);

			if (TmpFloor->IsClose())
			{
				continue;
			}

			float Distance = FVector::Dist(TmpFloor->GetActorLocation(), ActiveFloor->GetActorLocation());

			if (FMath::IsNearlyEqual(Distance, NearDistance))
			{
				if (FMath::RandBool())
				{
					DestinyFloor = TmpFloor;
				}
			}
			else if (Distance < NearDistance)
			{
				NearDistance = Distance;
				DestinyFloor = TmpFloor;
			}
		}
	}

	return DestinyFloor;
}


void ASN_AbstractDemon::DamageCharacter()
{
	if (!ActorToDamage)
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	IHealthInterface* HealthCharacter = Cast<IHealthInterface>(ActorToDamage);

	if (!HealthCharacter || (HealthCharacter->IsDeath()))
	{
		DemonEndActionsDelegate.Broadcast();
		return;
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(), ActorToDamage->GetActorLocation());
	SetActorRotation(LookAtRotation);

	if (IsValid(AttackMontage))
	{
		AnimInstance->PlayMontage(AttackMontage);
	}

	HealthCharacter->RecieveDamage(DamageValue);

	DemonEndActionsDelegate.Broadcast();
}

void ASN_AbstractDemon::RecieveDamage(int Damage)
{
	HealthComponent->RecieveDamage(Damage);
}

bool ASN_AbstractDemon::IsDeath() const
{
	return HealthComponent->IsDeath();
}

int ASN_AbstractDemon::GetMinimunForceToDamage() const
{
	return HealthComponent->GetMinimunForceToDamage();
}

int ASN_AbstractDemon::GetHealth() const
{
	return HealthComponent->GetCurrentHealth();
}

void ASN_AbstractDemon::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (ActionState == EDemonAction::Attack)
	{
		DamageCharacter();
	}
	else if (ActionState == EDemonAction::Move)
	{
		DemonEndActionsDelegate.Broadcast();
	}
}

void ASN_AbstractDemon::InitEndTurnDelegate()
{
	bInitEndTurnDelegate = true;
}

FVector ASN_AbstractDemon::GetLocation() const
{
	return GetActorLocation();
}

void ASN_AbstractDemon::EnableHealthWidget(bool bEnable)
{
	BP_EnableHealthWidget(bEnable);
}

inline void ASN_AbstractDemon::ActiveDemon()
{
	bActive = true;
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void ASN_AbstractDemon::AddHealth(int NewHealth)
{
	HealthComponent->AddHealth(NewHealth);
}

void ASN_AbstractDemon::SetHealth(int NewHealth)
{
	HealthComponent->SetHealth(NewHealth);
}

void ASN_AbstractDemon::PlayHitParticles()
{
	if (HitParticles)
	{
		FVector SocketLocation = GetMesh()->GetSocketLocation("Hit_Socket");
		FRotator SocketRotation = GetMesh()->GetSocketRotation("Hit_Socket");

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, SocketLocation, SocketRotation, true);
	}
}

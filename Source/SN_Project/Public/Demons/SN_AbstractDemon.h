// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemonInterface.h"
#include "Actors/IMovement.h"
#include "GameFramework/Character.h"
#include "SN_Components/HealthInterface.h"
#include "Delegates/DelegateCombinations.h"
#include "Navigation/PathFollowingComponent.h"
#include "SN_AbstractDemon.generated.h"

class USN_DemonAnimInstance;
class AAIController;
class USN_HealtComponent;
class UParticleSystemComponent;
class ASN_Floor;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathDemonDelegate, int, ExpValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDemonEndActionsDelegate);

UENUM(BlueprintType)
enum class EDemonAction : uint8
{
	None,
	Move,
	Attack
};

UCLASS()
class SN_PROJECT_API ASN_AbstractDemon : public ACharacter, public IHealthInterface, public IDemonInterface
{
	GENERATED_BODY()

protected:
	
	bool bIsExit;

private:

	int CountEnemiesAttackMe = 0;
	
	int NumActionsRemaing;
	
	AActor* TargetFloorAction;
	TArray<AActor*> aCharactersAction;

	bool bInitEndTurnDelegate;

	AAIController* DemonController;

	EDemonAction ActionState;
	AActor* ActorToDamage;

	USN_DemonAnimInstance* AnimInstance;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USN_HealtComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Demon")
	int NumActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Demon")
	int DamageValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demon")
	bool bActive;

	UPROPERTY(BlueprintReadOnly, Category = "Demon")
	bool bActiveSpawnAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Demon")
	int DeathExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Demon")
	float MoveRadiusToCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SpawnMontage;
	
	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* DeathParticles;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* SpawnParticles;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bDrawLineTraceEnemyEye;

	
public:

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnDeathDemonDelegate DeathDemonDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnDemonEndActionsDelegate DemonEndActionsDelegate;
	
public:
	// Sets default values for this character's properties
	ASN_AbstractDemon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateHudHealth();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateHudHealth();

	void DestroyDemon();
	
	IIMovement* GetFloorByNormalMovement(IIMovement* ActiveFloor, AActor* TargetActor);

	IIMovement* GetFloorVisibleCharacter(IIMovement* ActiveFloor, TArray<AActor*> aCharacters);

	UFUNCTION()
	void RealizeAction();

public:

	void Movement(AActor* TargetActor, const TArray<AActor*>& aCharacters, bool bCheckCharacters = true);
	
	UFUNCTION(BlueprintCallable, Category = "Demon")
	void Death();

	virtual void InicializeDemonValues() ;

	void DamageCharacter();

	virtual void RecieveDamage(int Damage) override;

	virtual bool IsDeath() const override;

	virtual int GetMinimunForceToDamage() const override;

	virtual int GetHealth() const override;

	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	void InitEndTurnDelegate();

	bool IsInitEndTurnDelegate() { return bInitEndTurnDelegate; }

	virtual FVector GetLocation() const override;

	virtual void EnableHealthWidget(bool bEnable);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_EnableHealthWidget(bool bEnable);

	virtual bool IsExit() const { return bIsExit; }

	virtual bool IsActive()  const override { return bActive; }

	virtual void ActiveDemon() override;

	void SetActiveSpawnAnimation(bool bActiveAnimation) { bActiveSpawnAnimation = bActiveAnimation; }

	virtual void AddHealth(int NewHealth) override;

	virtual bool IsInvisible() const override { return false; }

	virtual void SetInvisible(bool bNewInvisible) { }

	virtual void SetHealth(int NewHealth) override;

	UFUNCTION(BlueprintCallable)
	void PlayHitParticles();

	bool ExistCharacterToAttack();

	void Attack();

	int GetNumActions() const { return NumActions; }

	void PlaySpawnMontage();

	void PlaySpawnParticles();

	void ClearCountEnemiesAttackMe() override { CountEnemiesAttackMe = 0; }

	int GetCountEnemiesAttackMe() const override { return CountEnemiesAttackMe; }

	void AddCountEnemiesAttackMe() override { ++CountEnemiesAttackMe; }
	virtual FVector GetFreePostionToAttack();
};





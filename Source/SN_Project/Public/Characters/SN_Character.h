// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/CardsItems.h"
#include "GameFramework/Character.h"
#include "Player/SN_PlayerFP.h"
#include "SN_Components/HealthInterface.h"
#include "SN_Character.generated.h"

class USN_WCharacter;
class USN_CharacterAnimInstance;
class IIMovement;

namespace EPathFollowingResult
{
	enum Type : int;
}

class UStaticMeshComponent;
class UBillboardComponent;
class USN_HealtComponent;
class ASN_Floor;
class UAnimSequence;
class AAIController;
class USoundBase;
class UParticleSystem;
class IHealthInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeathDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterEndMovementDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterEndActionDelegate, bool, bDecrementAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterOpenDoorDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterLevelUpDelegate);

UCLASS()
class SN_PROJECT_API ASN_Character : public ACharacter, public IHealthInterface
{
	GENERATED_BODY()

private:
	AAIController* CharacterController;
	USN_CharacterAnimInstance* AnimInstance;

	FCardsItems CardUseInAttack;
	bool bCardToAttackIsLeft;

	IHealthInterface* ActorToDamageInAttack;
	IIMovement* FloorTargetAttack;

	USN_WCharacter* WCharacterRef;

	TArray<bool> aEnemyAttackPositionFree;

	/*
	 * Contador para saber cuantos enemigos van a atacarme y que no sean más
	 * del límite para evitar colisiones entre ellos
	 */
	int CountEnemiesAttackMe;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int NumActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int ExpCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SN")
	int LevelCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* SMPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UBillboardComponent* CameraLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USN_HealtComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* LeftWeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UStaticMeshComponent* RightWeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UParticleSystemComponent* LeftParticleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UParticleSystemComponent* RightParticleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UBillboardComponent* EnemyAttackPositionComponent1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UBillboardComponent* EnemyAttackPositionComponent2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UBillboardComponent* EnemyAttackPositionComponent3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UBillboardComponent* EnemyAttackPositionComponent4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	FCardsItems LeftItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	FCardsItems RightItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	FCardsItems BodyItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	bool bLeftItemInit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	bool bRightItemInit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Items")
	bool bBodyItemInit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move")
	float MoveRadiusToDemon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bImmortal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bNeverOpenDoor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Status")
	bool bEscape;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds")
	USoundBase* TeleportSoundCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds")
	USoundBase* SearchSoundCue;

	bool bInvisible;

	ETypeAction ActionState;

public:
	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnCharacterDeathDelegate FOnCharacterDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category="Delegate")
	FOnCharacterEndMovementDelegate CharacterEndMovementDelegate;

	UPROPERTY(BlueprintCallable, Category="Delegate")
	FOnCharacterOpenDoorDelegate CharacterOpenDoorDelegate;

	UPROPERTY(BlueprintCallable, Category="Delegate")
	FOnCharacterEndActionDelegate CharacterEndActionDelegate;

	UPROPERTY(BlueprintCallable, Category="Delegate")
	FOnCharacterLevelUpDelegate CharacterLevelUpDelegate;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* StartDashParticles;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* EndDashParticles;

public:
	// Sets default values for this character's properties
	ASN_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateHudHealth();

	bool SaveDamage();

	void CheckCharacterLevel();

	void RotateToActorToDamage();

	void RotateToDoor();

public:
	/*
	 * Actualiza el Widget de la vida del jugador
	 */
	UFUNCTION()
	void UpdateHudHealth();

	/*
	 * Activa o desactiva la marca de posición dentro del nivel para que el jugador
	 * pueda localizar rápidamente donde se encuentra el character activo.
	 */
	UFUNCTION(BlueprintCallable)
	void EnableDisableShowPosition(bool bEnabled);

	/*
	 * Lógica de muerte del character
	 */
	UFUNCTION(BlueprintCallable)
	void Death();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Death();
	
	/*
	 * Muestra la casilla donde se encuentra el character
	 */
	UFUNCTION(BlueprintCallable)
	void ShowActiveFloors();

	/*
	 * Mueve al character a la casilla indicada
	 */
	UFUNCTION(BlueprintCallable)
	void MoveToFloor(AActor* FloorRef);

	/*
	 * Mueve al character a la posición indicada con un radio de margen
	 */
	UFUNCTION(BlueprintCallable)
	void MoveToLocation(FVector TargetLocation, float Radius);

	/*
	 * Añade un Item a las manos del jugador
	 */
	UFUNCTION(BlueprintCallable)
	void AddItem(bool bLeft, FCardsItems NewItem);

	/*
	 * Añade un Item al cuerpo del jugador (Armaduras)
	 */
	UFUNCTION(BlueprintCallable)
	void AddBodyItem(FCardsItems NewItem);

	/*
	 * Indica si puede abrir puertas según los items de los que disponga
	 */
	UFUNCTION(BlueprintPure)
	bool CanOpenDoors();

	/*
	 * Devuelve el valor mínimo de abrir puerta de los items que dispnga
	 */
	UFUNCTION(BlueprintPure)
	int GetMinValueItemOpenDoors();

	/*
	 * Indica si puede atacar según los items de los que disponga
	 */
	UFUNCTION(BlueprintPure)
	bool CanAttack();

	/*
	 * Indica el número de acciones disponibles en el turno actual
	 */
	int GetNumActions();

	/*
	 * Indica si dispone de un item en la mano izquierda
	 */
	bool IsLeftItemInit() { return bLeftItemInit; }

	/*
	 * Indica si dispone de un item en la mano derecha
	 */
	bool IsRightItemInit() { return bRightItemInit; }

	/*
	 * Indica si dispone de un item en el cuerpo
	 */
	bool IsBodyItemInit() { return bBodyItemInit; }

	FCardsItems GetLeftItem() const { return LeftItem; }

	void ClearAllItems();

	FCardsItems GetRightItem() const { return RightItem; }

	FCardsItems GetBodyItem() const { return BodyItem; }

	virtual void RecieveDamage(int Damage) override;
	UFUNCTION(BlueprintPure)
	virtual bool IsDeath() const override;

	virtual int GetMinimunForceToDamage() const override;

	virtual void AddHealth(int NewHealth) override;

	virtual void SetHealth(int NewHealth) override;

	virtual bool IsInvisible() const override { return bInvisible; }

	/*
	 * Activa o desactiva la mágia de hacerte invisible de los enemigos
	 */
	virtual void SetInvisible(bool bNewInvisible) { bInvisible = bNewInvisible; }

	UFUNCTION(BlueprintPure)
	virtual int GetHealth() const override;

	void AddExp(int NewExp);

	int GetExp() const { return ExpCharacter; }

	void SetExp(int NewExp) { ExpCharacter = NewExp; }

	/*
	 * Indica si el character a conseguido escapar del nivel
	 */
	bool IsEscape() const { return bEscape; }

	void SetEscape(bool NewEscape) { bEscape = NewEscape; }

	/*
	 * Se ejecuta cuando el character acaba de realizar el movimiento hacia la posición indicada.
	 */
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	virtual FVector GetLocation() const override;

	void OpenDoor();

	void RealizeAttack(FCardsItems CardToAttack, IIMovement* FloorTarget, IHealthInterface* ActorToDamage,
	                   bool bLeftCard);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_LaunchMagic(const FCardsItems& CardToAttack, AActor* FloorTarget,
								  AActor* ActorToDamage, bool bLeftHand);

	UFUNCTION(BlueprintCallable)
	void RealizeDamageReturnBP(const FCardsItems& CardToAttack, AActor* FloorTarget, AActor* ActorToDamage);

	
	void RealizeDamage(const FCardsItems& CardToAttack, IIMovement* FloorTarget, IHealthInterface* ActorToDamage);

	void CheckOpenDoor();

	void RealizeAnimation(EAnimationType AnimationType, bool bLeft);

	bool RealizeFriendDamage(FCardsItems CardToAttack, TArray<AActor*> DamageActors);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AnimOpenDoor();

	UFUNCTION(BlueprintCallable)
	void CheckMeleeWeaponImpact();

	void AttachWeaponCard(FCardsItems Item, bool bHandLeft);

	UFUNCTION(BlueprintCallable)
	void SetWCharacterRef(USN_WCharacter* NewWCharacterRef) { WCharacterRef = NewWCharacterRef; };

	UFUNCTION(BlueprintPure)
	USN_WCharacter* GetWCharacterRef() { return WCharacterRef; }

	void PlayVFXStartDash();

	void PlayVFXEndDash();

	void PlayVFXSearch();

	UFUNCTION(BlueprintPure)
	ETypeAction GetActionState() { return ActionState; }

	void SetActionState(ETypeAction NewActionState) { ActionState = NewActionState; }

	void ClearCountEnemiesAttackMe() ;

	virtual int GetCountEnemiesAttackMe() const override { return CountEnemiesAttackMe; }

	void AddCountEnemiesAttackMe() override;

	virtual FVector GetFreePostionToAttack();
	
};

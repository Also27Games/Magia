// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/CardsItems.h"
#include "GameFramework/Character.h"
#include "Actors/IMovement.h"
#include "SN_PlayerFP.generated.h"

class IDemonInterface;
class IHealthInterface;
struct FInputActionValue;
class ASN_GameMode;
class ASN_Character;
class UInputAction;
class UCameraComponent;
class UInputMappingContext;
class USceneComponent;
class UFloatingPawnMovement;

UENUM(BlueprintType)
enum class ETypeAction : uint8
{
	None,
	Move,
	OpenDoor,
	Search,
	Attack
};

UCLASS()
class SN_PROJECT_API ASN_PlayerFP : public ACharacter
{
	GENERATED_BODY()

private:
	APlayerController* PlayerController;

	AActor* LastEnemyOverlap;

	bool bHasSearched;

	bool bHasHealing;
	bool bHasInvisiblity;
	bool bHasSpeed;
	bool bRealizeAttackOnEndMovement;

	int ValueOpenDoorAction;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Movement")
	bool bActiveMovement;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Movement")
	bool bEnableCameraMovement;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Refs")
	ASN_GameMode* GMRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Actions")
	ETypeAction Action;

	FCardsItems ActiveCardToAttack;
	bool bCardToAttackIsLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bDisableMoveCameraToCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bDamageAlways;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bTeleportationActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bInifiniteTurns;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bInifiniteMagic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Debug")
	bool bInifiniteSearch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Actions")
	int ActionsRemaining;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FindCharacterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ActiveMovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UFloatingPawnMovement* FloatingPawnMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Sounds")
	USoundBase* OpenDoorSoundCue;

public:
	// Sets default values for this pawn's properties
	ASN_PlayerFP();

private:
	UFUNCTION(BlueprintCallable)
	void SetupGameAndUIMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);

	void MoveUp(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_Look(float X, float Y);

	void EnableMovement();
	void DisableMovement();

	/*
	 * Esconde el Hud de la pantalla 
	 */
	UFUNCTION()
	void RemoveHud();

	/*
	 * Actualiza el hud de nivel de partida
	 */
	UFUNCTION(BlueprintCallable)
	void UpdateGameLevelHud();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateGameLevelHud();


	UFUNCTION()
	void InitDemonTurn();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitDemonTurn();

	UFUNCTION()
	void EndDemonTurn();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_EndDemonTurn();

	/*
	 * Inicializa los Delegates necesarios
	 */
	void InitDelegates();

	/*
	 * Muestra el HUD en pantalla
	 */
	UFUNCTION(BlueprintCallable)
	void ShowPlayerHud(bool bShowAll = false);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ShowPlayerHud(bool bShowAll);

	/*
	 * Se ejecuta cuando el character activo acaba de realizar el movimiento indicado
	 */
	UFUNCTION()
	void CharacterEndMovement();

	UFUNCTION()
	void CharacterEndAction(bool bDecrementAction);

	/*
	 * Muestra el Widget de los demons cuando el cursor hace overlap con ellos
	 */
	void ShowHealthWidgetDemons();

	/*
	 * Realiza la acción de ataque
	 */
	void AttackAction();

	/*
	 * Realiza la acción de mágia
	 */
	void MagicAction();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_ShowPlayerMessageInHud(const FString& Message);

	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*
	 * Inicializa las referencias a los objetos necesarios
	 */
	void InitRefs();

	/*
	 * Devuele el character activo en el turno
	 */
	UFUNCTION(BlueprintPure, BlueprintCallable, Category="GM")
	ASN_Character* GetActiveCharacter() const;

	/*
	 * Devuele la casilla del character activo del turno
	 */
	IIMovement* GetFloorActiveCharacter();

	/*
	 * Inicia la acción de movimiento (muestra las casillas activas para poder moverse)
	 */
	UFUNCTION(BlueprintCallable)
	void InitMoveAction();

	/*
	 * Realiza el movimiento del character activo hacia la casilla seleccionada
	 */
	UFUNCTION(BlueprintCallable)
	bool MoveToFloor();

	/*
	 * Resta uno a las acciones disponibles en el turno
	 */
	UFUNCTION(BlueprintCallable)
	void DecrementAction();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateActionsInHud();

	/*
	 * Finaliza el turno del character activo y salta al siguiente character o inicia el turno de los
	 * demonios si este fuera el último character de la ronda
	 */
	UFUNCTION(BlueprintCallable)
	void FinishTurn();

	/*
	 * Inicializa los valores del character en el turno
	 */
	UFUNCTION()
	void InitCharacterValues();

	/*
	 * Show message to player when game level up
	 */
	UFUNCTION()
	void ShowGameLevelUp();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitCharacterHud();

	/*
	 * Mueve la camara al character indicado
	 */
	UFUNCTION(BlueprintCallable)
	void MoveCameraToCharacter(ASN_Character* Character);

	/*
	 * Mueve la camara al character activo
	 */
	void MoveCameraToCharacter();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_MoveCameraToCharacter(ASN_Character* Character);

	/*
	 * Inicializa los Items del character activo en el Hud del jugador
	 */
	UFUNCTION(BlueprintCallable)
	void InitCharacterCards();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateCardsInHud(bool bLeft, FCardsItems CardItem);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateBodyCardInHud(FCardsItems CardItem);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ClearCardsInHud();

	/*
	 * Evento de realizar click en pantalla con el ratón y actua según valor de la máquina de estados (Action)
	 */
	UFUNCTION(BlueprintCallable)
	void ClickActionFunction();

	/*
	 * Realiza la acción de buscar
	 */
	UFUNCTION(BlueprintCallable)
	void SearchAction();

	UFUNCTION()
	void ShowSearchAction();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_SearchAction(FCardsItems CardItem);

	/*
	 * Realiza la acción de abrir la puerta de la casilla donde se encuentra el character activo
	 */
	UFUNCTION(BlueprintCallable)
	void OpenDoorAction();

	/*
	 * Cancela la acción iniciada
	 */
	UFUNCTION(BlueprintCallable)
	void CancelAction();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_CancelAction(bool bComeFromAttack);

	/*
	 * Realiza la acción de movimiento
	 */
	void MovementAction();

	/*
	 * Inicia la acción de ataque (muestra los Items que tiene disponible para realizar el ataque)
	 */
	UFUNCTION(BlueprintCallable)
	void InitAttackAction();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitAttackAction(FCardsItems LeftItem, FCardsItems RightItem, bool bShowCards);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_HidePlayerHud(bool bHideAll);

	/*
	 * Oculta el Hud del jugador
	 */
	UFUNCTION(BlueprintCallable)
	void HidePlayerHud(bool bHideAll);

	/*
	 * Muestra las casillas donde el jugador puede atacar según el Item seleccionado
	 */
	UFUNCTION(BlueprintCallable)
	void ShowFloorsAttack(FCardsItems CardItem, bool bLeftCard);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ShowFloorsAttack();

	/*
	 * Realiza la acción de ataque
	 */
	void RealizeAttack(IIMovement* Floor, IHealthInterface* ActorToDamage);

	/*
	 * Devuelve el actor que ha recibido el click del ratón
	 */
	AActor* GetActorClick(ECollisionChannel CollisionChannel) const;

	/*
	 * Devuelve Locator que ha recibido el click del ratón
	 */
	FVector GetLocationClick(const ECollisionChannel CollisionChannel) const;

	/*
	 * Realiza la acción de coger objetivo
	 */
	UFUNCTION(BlueprintCallable)
	void TakeObjective();

	/*
	 * Show player message in hud
	 */
	void ShowPlayerMessage(const FString& Message);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_AddWidgetCharacter(UUserWidget* Widget);
	
};



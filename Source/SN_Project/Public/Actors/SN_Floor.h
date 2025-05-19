// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IMovement.h"
#include "GameFramework/Actor.h"
#include "SN_Floor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USceneComponent;
class ASN_Door;
class ACharacter;
class UCameraComponent;
class UBillboardComponent;

UCLASS()
class SN_PROJECT_API ASN_Floor : public AActor, public IIMovement
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* FloorRootComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SMFloor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SMViewActiveFloorLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SMViewActiveFloorRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DetectPlayerCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DetectDemonCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* DetectFloorCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* OpenDoorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* FloorCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	bool bActiveMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor")
	bool bClose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor")
	ASN_Door* DoorRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor")
	bool bCanSearch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floor")
	bool bSpawnDemonsInFloor;
	
	
public:	
	// Sets default values for this actor's properties
	ASN_Floor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	void SetState(bool bNewState);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_SetState(bool bNewState);

	UFUNCTION(BlueprintCallable)
	void ResetMovement();

	UFUNCTION(BlueprintCallable)
	virtual void OpenDoor() override;

	/*
	 * Devuelve la posición de la puerta que existe dentro de la casilla
	 */
	virtual FVector GetDoorLocation() override;

	/*
	 * Devuelve si en la casilla existe una puerta para abrir o no
	 */
	virtual bool ExistDoor() override;

	/*
	 * Devuelve todas las casillas con la que colinda
	 */
	UFUNCTION(BlueprintPure)
	virtual TArray<AActor*> GetOverlappingFloors() const override;

	/*
	 * Devuelve todos los characters que están dentro de la casilla
	 */
	UFUNCTION(BlueprintPure)
	virtual TArray<AActor*> GetOverlappingCharacters() const override;

	/*
	 * Devuelve todos los demonios que están dentro de la casilla
	 */
	UFUNCTION(BlueprintPure)
	virtual TArray<AActor*> GetOverlappingDemons() const override;

	/*
	 * Indica si la casilla esta activa para poder moverse a ella o no
	 */
	virtual bool IsActiveMovement() override { return bActiveMovement; }

	/*
	 * Indica si la casilla está cerrada y no se puede entrar.
	 */
	virtual bool IsClose() override { return bClose; }

	virtual void SetClose(bool bNewClose) override { bClose = bNewClose; }

	UFUNCTION(BlueprintCallable, Category = "BPI_Movement")
	virtual void InitMovement() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_InitMovement();

	/*
	 * Activa o desactiva la casilla para permitir movimiento hacia ella.
	 */
	UFUNCTION(BlueprintCallable, Category = "BPI_Movement")
	virtual void EnableDisableMovement(bool bEnable) override;

	/*
	 * Activa el movimiento de las casillas colindantes para poder moverse a ellas.
	 */
	virtual void ShowActiveFloorsToMovement() override;

	/*
	 * Devuelve las casillas a las que se permite atacar dependiendo de la distancia del arma.
	 */
	virtual void GetFloorsToAttack(TArray<IIMovement*>& aFloorsReturn, IIMovement* const ActualFloor, int Distance) override;

	/*
	 * Indica si en la casilla se puede buscar un Item o no
	 */
	virtual bool CanSearch() override;

	/*
	 * Devuelve un Location aleatorio dentro de la casilla
	 */
	virtual FVector GetRandomLocationInFloor() override;

	/*
	 * Devuelve el GetActorLocation (Método implementado desde la Interface IMovement, asi podemos saber su posición sin
	 * hacer cast al objeto
	 */
	virtual  FVector GetActorLocation() override;

	/*
	 * Devuelve el GetActorRotation (Método implementado desde la Interface IMovement, asi podemos saber su rotación sin
	 * hacer cast al objeto
	 */
	virtual  FRotator GetActorRotation() override;

	/*
	 * Devuelve si la casilla permite spawnear demonios en ella.
	 */
	virtual bool CanSpawnDemonsInFloor() override;

	/*
	 * Devuelve si en la casilla existe algún objetivo (misión) a cumplir
	 */
	virtual AActor* GetObjectiveActor() override;

	virtual FVector GetLocationOpenDoor() override;

	virtual  void PlayVFXHitDoor();
	
};



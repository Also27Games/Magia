// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SN_TemporalCamera.generated.h"

class UCameraComponent;

UCLASS()
class SN_PROJECT_API ASN_TemporalCamera : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	USceneComponent* TmpCameraRootComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components")
	UCameraComponent* Camera;

public:
	// Sets default values for this actor's properties
	ASN_TemporalCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};

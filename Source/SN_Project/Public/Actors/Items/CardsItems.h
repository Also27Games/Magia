// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CardsItems.generated.h"

/**
 * 
 */

class ASN_Projectile;

UENUM(BlueprintType)
enum class ETypeMagic : uint8
{
	None,
	Health,
	Speed,
	Invisiblity,
	Attack
};

UENUM(BlueprintType)
enum class EAnimationType : uint8
{
	None,
	Light,
	Heavy,
	Bow,
	Magic
};

class UStaticMesh;

/*
 * Estrucutra de datos para los Items de la partida
*/
USTRUCT(BlueprintType)
struct SN_PROJECT_API FCardsItems: public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Id;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bInitWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int NumInitWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int InitItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int CurrentItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bZombie;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bAttackMelee;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bAttackDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bRequiereMovement;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bMagic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	ETypeMagic TypeMagic;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Dice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Hit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bNoise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bDoor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int DiceDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bNoiseDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bDouble;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSoftObjectPtr<UTexture2D> Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	UParticleSystem* Particles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	UParticleSystem* EffectParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	USoundBase* SoundEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EAnimationType AnimationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TSubclassOf<ASN_Projectile> ProjectileRef;	
};

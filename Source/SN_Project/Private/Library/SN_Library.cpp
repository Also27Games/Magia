// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/SN_Library.h"

#include "CollisionQueryParams.h"
#include "Actors/SN_Floor.h"
#include "Engine/HitResult.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SN_PlayerFP.h"

ASN_Floor* USN_Library::GetActiveFloor(const AActor* Target) 
{

	if (!Target)
	{
		return nullptr;
	}

	FVector StartTargetLocation = Target->GetActorLocation();
	FVector EndTargetLocation = Target->GetActorLocation();

	EndTargetLocation.Z = -100.f;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	
	bool bHit = Target->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartTargetLocation,
		EndTargetLocation,
		ECC_GameTraceChannel1,  // Canal de colisión: Visibilidad
		TraceParams
	);

	if (!bHit)
	{
		return nullptr;
	}
	
	return Cast<ASN_Floor>(HitResult.GetActor());
	
}

AActor* USN_Library::GetActiveActorFloor(const AActor* Target)
{

	if (!Target)
	{
		return nullptr;
	}

	FVector StartTargetLocation = Target->GetActorLocation();
	FVector EndTargetLocation = Target->GetActorLocation();

	EndTargetLocation.Z = -100.f;

	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	
	bool bHit = Target->GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartTargetLocation,
		EndTargetLocation,
		ECC_GameTraceChannel1,
		TraceParams
	);

	FColor LineColor = bHit ? FColor::Green : FColor::Red;

	/*DrawDebugLine(
		Target->GetWorld(),
		StartTargetLocation,
		EndTargetLocation,
		LineColor,
		false,          // No permanece en el mundo para siempre
		2.0f,           // Duración de la línea en segundos
		0,
		3.0f            // Grosor de la línea
	);*/
	
	if (!bHit)
	{
		return nullptr;
	}
	
	return HitResult.GetActor();
	
}

int USN_Library::GetRandomDiceNumber()
{
	return FMath::RandRange(1, 6);
}

void USN_Library::ShowMessageToPlayer(const UObject* WorldContext, const FString& Message)
{

	if (!IsValid(WorldContext))
	{
		return;
	}

	ASN_PlayerFP* PlayerFP = Cast<ASN_PlayerFP>(UGameplayStatics::GetPlayerCharacter(WorldContext, 0));

	if (!IsValid(PlayerFP))
	{
		return;
	}

	PlayerFP->ShowPlayerMessage(Message);
	
}

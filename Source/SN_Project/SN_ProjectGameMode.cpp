// Copyright Epic Games, Inc. All Rights Reserved.

#include "SN_ProjectGameMode.h"
#include "SN_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASN_ProjectGameMode::ASN_ProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

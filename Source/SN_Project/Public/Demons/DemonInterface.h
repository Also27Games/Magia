// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DemonInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDemonInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SN_PROJECT_API IDemonInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/*
	 * Muestra o oculta el widget con los stats del demonio
	 */
	virtual void EnableHealthWidget(bool bEnable) = 0;

	virtual bool IsActive() const = 0;

	/*
	 * Activa la lógica del demonio en su turno
	 */
	virtual void ActiveDemon() = 0;
	
};

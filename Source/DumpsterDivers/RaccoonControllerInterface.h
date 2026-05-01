// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RaccoonControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URaccoonControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUMPSTERDIVERS_API IRaccoonControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AddItemToInventory(int index) = 0;
};

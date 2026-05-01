// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pig.generated.h"

// This class is unused 
UCLASS()
class DUMPSTERDIVERS_API APig : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APig();
	FGuid PigFGuid;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintImplementableEvent)
	void SetCollisionActive(bool Toggle);

};

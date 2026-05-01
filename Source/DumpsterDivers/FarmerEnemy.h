// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Enemy.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

#include "FarmerEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API AFarmerEnemy : public AEnemy
{
	GENERATED_BODY()

	public:
	AFarmerEnemy();

	virtual void Attack() override;

	protected:
	
};

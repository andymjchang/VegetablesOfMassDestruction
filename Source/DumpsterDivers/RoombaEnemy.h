// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.h"
#include "RoombaEnemy.generated.h"

UCLASS()
class DUMPSTERDIVERS_API ARoombaEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	ARoombaEnemy();

	void Recoil();

	void SpawnBP(const TSubclassOf<AActor>& SpawnActor, const FVector& SpawnOffset = FVector::ZeroVector) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TSubclassOf<AActor> projectile;
	
	virtual void Attack() override;
protected:
};

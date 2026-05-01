// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventorySubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerDefaults.generated.h"

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float MaxHealth;

	UPROPERTY(EditAnywhere)
	float CurrentHealth;

	UPROPERTY(EditAnywhere)
	int Power;

	UPROPERTY(EditAnywhere)
	int CritChance;

	UPROPERTY(EditAnywhere)
	float CritMultiplier;

	UPROPERTY(EditAnywhere)
	float Speed;

	UPROPERTY(EditAnywhere)
	float AttackSpeed;

	UPROPERTY(EditAnywhere)
	int EggBaseDamage;

	UPROPERTY(EditAnywhere)
	int EggBaseRadius;

	UPROPERTY(EditAnywhere)
	TArray<float> DashCooldown;
    
	UPROPERTY(EditAnywhere)
	TArray<int> MaxDashCount;

	UPROPERTY(EditAnywhere)
	TArray<FWeaponStruct> Inventory;

	FPlayerData() :
		MaxHealth(100.f),
		CurrentHealth(100.f),
		Power(5),
		CritChance(10),
		CritMultiplier(1.5f),
		Speed(1.f),
		AttackSpeed(1.f),
		EggBaseDamage(30),
		EggBaseRadius(768),
		DashCooldown({1.f, 5.f, 5.f}),
		MaxDashCount({1, 1, 1}),
		Inventory()
	{}
	
};

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UPlayerDefaults : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static const FPlayerData DefaultPlayerData;
};

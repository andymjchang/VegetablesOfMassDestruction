// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerDefaults.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API USaveDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable)
	void SaveData();

	UFUNCTION(BlueprintCallable)
	void LoadData();

	UFUNCTION(BlueprintCallable)
	void ClearData();

	UFUNCTION(BlueprintCallable)
	void ModifyHealth(float Health);
	
protected:
	UPROPERTY(EditAnywhere)
	FPlayerData PlayerData;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> CompanionsData;
};

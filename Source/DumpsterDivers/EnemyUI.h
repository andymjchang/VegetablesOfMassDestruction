// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

#include "EnemyUI.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UEnemyUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateHealthBar(float HealthPercentage);

	UFUNCTION(BlueprintCallable)
	void UpdateArmorBar(float ArmorPercentage);

	UFUNCTION(BlueprintCallable)
	void ShowArmorBar();

	UFUNCTION(BlueprintCallable)
	void HideArmorBar();

protected:
	UPROPERTY()
	FName HealthBarName = "HealthBar";
	
	UPROPERTY()
	FName ArmorBarName = "ArmorBar";
};

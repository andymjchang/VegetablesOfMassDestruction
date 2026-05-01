// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AugmentGUI.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RaccoonGUI.h"
#include "Kismet/GameplayStatics.h"
#include "DefaultGameModeBase.h"
#include "EnemyIndicatorUI.h"
#include "GUISubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UGUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URaccoonGUI* RaccoonGUI; 
	
	UPROPERTY()
	UAugmentGUI* AugmentGUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnemyIndicatorUI* EnemyIndicatorUI;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE URaccoonGUI* GetRaccoonGUI() { return RaccoonGUI; }

	UFUNCTION(BlueprintCallable)
	void InitializeGUIs(TSubclassOf<UEnemyIndicatorUI> EnemyIndicatorUIClass);
	
};

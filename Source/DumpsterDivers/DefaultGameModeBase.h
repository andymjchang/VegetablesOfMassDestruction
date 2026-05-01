// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyIndicatorUI.h"
#include "GameFramework/GameModeBase.h"
#include "RaccoonGUI.h"
#include "GameManagerInstance.h"
#include "Engine/DataTable.h"


#include "DefaultGameModeBase.generated.h"
class UGUISubsystem; 
class UInventorySubsystem; 
/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API ADefaultGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ADefaultGameModeBase();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* weaponTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* tagVFXTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UEnemyIndicatorUI> EnemyIndicatorUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FadeInDuration;
};

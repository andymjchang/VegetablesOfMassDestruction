// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemySpawner.h"
#include "SectionDoor.h"
#include "DumpsterDivers/EnemyIndicatorUI.h"
#include "Subsystems/WorldSubsystem.h"
#include "LevelManagerSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSectionEntered, int, SectionNumber);

UCLASS()
class DUMPSTERDIVERS_API ULevelManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void RegisterDoor(ASectionDoor* NewDoor);
	
	UFUNCTION(BlueprintCallable)
	void InitLevelManager();

	UFUNCTION(BlueprintCallable)
	void StartSection(int SectionNumber);

	UFUNCTION(BlueprintCallable)
	void EnemyDefeatedEvent(AEnemy* Enemy);

	FORCEINLINE int GetCurrentSectionID() const { return CurrentSectionID; };

	TArray<AEnemy*>* GetEnemiesInSection(int SectionID);

protected:
	UPROPERTY()
	TArray<ASectionDoor*> Doors;

	UPROPERTY()
	UEnemyIndicatorUI* EnemyIndicatorUI;

	// Number of Enemies active in each section
	// TArray<int> EnemyCounts;

	TArray<TArray<AEnemy*>> Enemies;
	TArray<TArray<AEnemySpawner*>> Spawners;

	FTimerHandle SpawnTimer;

	int CurrentSectionID;

	void CollectSpawnersBySection();
	void CountPrePlacedEnemies();
	void SpawnUntilFull(int SectionIndex);
	AEnemySpawner* SelectRandomSpawner(int SectionIndex);
	void CheckForSectionComplete(int SectionIndex);
	void FetchEnemyIndicatorUI();
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSectionEntered OnSectionEntered;
};

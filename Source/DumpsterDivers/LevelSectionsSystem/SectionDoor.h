// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/BezierUtilities.h"
#include "DumpsterDivers/Enemy.h"
#include "GameFramework/Actor.h"
#include "SectionDoor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSectionStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSectionEnd);

USTRUCT(BlueprintType)
struct FEnemySpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle ItemData;
};

UCLASS()
class DUMPSTERDIVERS_API ASectionDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASectionDoor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Section Settings")
	int SectionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Section Settings")
	int MaxActiveEnemies;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Section Settings")
	float SpawnDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Section Settings")
	bool RandomizeNextEnemySpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Section Settings")
	TArray<FEnemySpawnConfig> EnemySpawnList;

	FOnSectionStart OnSectionStart;
	FOnSectionEnd OnSectionEnd;
	
	TQueue<FEnemySpawnConfig> EnemyQueue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnEnterDoor(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* otherComponent, 
						  int32 OtherIndex, bool bFromSweep, const FHitResult &Hit);
	
	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Rotation")
	float RotationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Rotation")
	FRotator RotationAngleDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Rotation")
	float CurveSteepness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings|Rotation")
	float CurveMidpoint;

	float RotationTimeElapsed;

	FRotator OriginalRotation;
	FRotator InitialRotation;
	FRotator TargetRotation;
	
	bool IsMoving;
	bool IsOpened;
	bool EventQueued;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BlockingBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* RotationPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* DoorMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UArrowComponent* DoorRotationAxis;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OpenDoor();

	UFUNCTION(BlueprintCallable)
	void CloseDoor();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetSectionID() const { return SectionID; }	

	UFUNCTION(BlueprintImplementableEvent)
	void SectionStartEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void SectionEndEvent();
};

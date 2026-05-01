// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Crate.generated.h"

UCLASS()
class DUMPSTERDIVERS_API ACrate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrate();

	UFUNCTION(BlueprintCallable)
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	           UPrimitiveComponent* otherComponent, int32 OtherIndex,
	           bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void SetItemData(FDataTableRowHandle NewItemData);
	
protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Durability;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGeometryCollection* GeometryCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClearCrateDebrisDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchAngleDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchTorque;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayUntilInteractableSeconds;

	FTimerHandle ItemTimer;
	FTimerHandle CleanupTimer;
	bool IsActive;
	
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CrateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* HurtBox;

	UFUNCTION(BlueprintCallable)
	void BreakCrate(AActor* OtherActor);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UStaticMeshComponent* GetCrateMesh() { return CrateMesh; }
	FORCEINLINE UBoxComponent* GetHurtBox() { return HurtBox; }

};

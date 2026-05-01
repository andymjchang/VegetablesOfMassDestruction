// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DataTable.h"
#include "Raccoon.h"
#include "Item.h"

#include "Breakable.generated.h"

UCLASS()
class DUMPSTERDIVERS_API ABreakable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakable();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HurtBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (AllowPrivateAccess = "true"))
	FDataTableRowHandle ItemDataTableRow;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable) 
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* otherComponent, int32 OtherIndex, bool bFromSweep, const FHitResult &Hit);

	FTimerHandle ReactivateTimer;
	FTimerHandle ItemTimer;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE class UBoxComponent* GetBoxComponent() const { return HurtBox; }
	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (AllowPrivateAccess = "true"))
	float LaunchAngleDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (AllowPrivateAccess = "true"))
	float LaunchMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (AllowPrivateAccess = "true"))
	float LaunchTorque;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (AllowPrivateAccess = "true"))
	float DelayUntilInteractableSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (AllowPrivateAccess = "true"))
	bool CanRespawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (EditCondition = "CanRespawn"))
	float DelayUntilRespawnSeconds;

private:

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Raccoon.h"
#include "Engine/TriggerBox.h"
#include "EndLevelTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API AEndLevelTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AEndLevelTriggerBox();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> NextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FadeOutDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoWalkSpeed;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:
	UPROPERTY()
	ARaccoon* Player;
	
	FTimerHandle OpenNextLevelTimer;
	bool AutoWalking;
	FVector MovementDirection;

	void WalkTowardsCenter(AActor* Actor);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskJumps.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UBTTaskJumps : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskJumps();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// UPROPERTY(EditAnywhere)
	// FBlackboardKeySelector RaccoonKey;
	//
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector SeeRaccoonKey;

	UPROPERTY(EditAnywhere, Category = "View Distance")
	float ViewDistance = 1000.0f;
	// UPROPERTY()
	// AActor* PlayerActor;
};

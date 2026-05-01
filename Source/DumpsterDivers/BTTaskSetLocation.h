// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskSetLocation.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UBTTaskSetLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskSetLocation();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector RaccoonKey;

	// UPROPERTY()
	// AActor* PlayerActor;
};

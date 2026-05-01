// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskFindPlayer.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UBTTaskFindPlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskFindPlayer();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector RaccoonKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector SeeRaccoonKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector InRangeKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category = "View Distance")
	float ViewDistance = 1000.0f;
};

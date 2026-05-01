// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "FarmerEnemy.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public: 
	AEnemyController();
	
	FORCEINLINE UBlackboardComponent * GetBlackboardComponent() const { return BlackboardComponent; }

private: 
	class UBehaviorTreeComponent * BehaviorComponent; 
	class UBlackboardComponent * BlackboardComponent; 

	virtual void OnPossess(APawn * InPawn) override; 

	
};

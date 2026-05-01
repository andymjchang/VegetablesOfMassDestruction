// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "CompanionController.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API ACompanionController : public AAIController
{
	GENERATED_BODY()

public: 
	ACompanionController();
	
	FORCEINLINE UBlackboardComponent * GetBlackboardComponent() const { return BlackboardComponent; }

private: 
	class UBehaviorTreeComponent * BehaviorComponent; 
	class UBlackboardComponent * BlackboardComponent; 

	virtual void OnPossess(APawn * InPawn) override; 

	
	
};

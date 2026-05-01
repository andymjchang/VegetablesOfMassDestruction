// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Suck.generated.h"


struct FBTTaskSuckMemory
{
	UBehaviorTreeComponent* BTComp;
	AActor* PlayerActor;
	AEnemy* EnemyActor;
	UBlackboardComponent* BlackboardComp;
	bool telegraphflag = false;
};

UCLASS()
class DUMPSTERDIVERS_API UBTT_Suck : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_Suck();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTTaskSuckMemory); }

	UPROPERTY()
	AActor* PlayerActor;

	UPROPERTY()
	AEnemy* EnemyActor;

	UPROPERTY()
	UBehaviorTreeComponent* BTComp;

	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float ChargeTime = 1.0f;

private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector RaccoonKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackEndTime;

	void Telegraph(FBTTaskSuckMemory* Memory, APawn* Enemy, float DeltaSeconds) const;

	void OnChargeComplete(UBehaviorTreeComponent* OwnerComp, FBTTaskSuckMemory* Memory) const;
};

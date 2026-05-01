// BTTaskAttack.h
#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskAttack.generated.h"

struct FBTTaskAttackMemory
{
	UBehaviorTreeComponent* BTComp;
	AActor* PlayerActor;
	AEnemy* EnemyActor;
	UBlackboardComponent* BlackboardComp;
	bool telegraphflag = false;
};

UCLASS()
class DUMPSTERDIVERS_API UBTTaskAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskAttack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTTaskAttackMemory); }

private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector RaccoonKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttackEndTime;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	float ChargeTime = 1.0f;
	
	void OnChargeComplete(UBehaviorTreeComponent* OwnerComp, FBTTaskAttackMemory* Memory) const;
	
	UPROPERTY()
	AActor* PlayerActor;

	UPROPERTY()
	AEnemy* EnemyActor;

	UPROPERTY()
	UBehaviorTreeComponent* BTComp;

	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	void Telegraph(FBTTaskAttackMemory* Memory, APawn* Enemy, float DeltaSeconds);
};
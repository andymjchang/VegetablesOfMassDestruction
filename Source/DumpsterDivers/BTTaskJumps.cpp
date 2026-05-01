#include "BTTaskJumps.h"
#include "AIController.h"
#include "Companion.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


UBTTaskJumps::UBTTaskJumps()
{
	NodeName = "Jumps";
	bNotifyTick = true;
}
EBTNodeResult::Type UBTTaskJumps::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get the AI Controller
	// Get the companion that uses this bb

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	ACompanion* companion = Cast<ACompanion>(AIController->GetPawn());
	if (!companion) return EBTNodeResult::Failed;




	// update the distance between the player and the companion
	
	APawn* raccoon = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!raccoon) return EBTNodeResult::Failed;
	
	auto blackboard = OwnerComp.GetBlackboardComponent();
	
	float distanceToPlayer = FVector::Dist(companion->GetActorLocation(), raccoon->GetActorLocation());
	if (distanceToPlayer > ViewDistance)
	{
		if (blackboard) blackboard->SetValueAsBool(SeeRaccoonKey.SelectedKeyName, false);
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("jump false")));
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("jump false%f"), distanceToPlayer));
		return EBTNodeResult::Failed; 
	}
	
	if (blackboard) blackboard->SetValueAsBool(SeeRaccoonKey.SelectedKeyName, true);
	// {
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("jump true")));
	
	companion->Jump();
	return EBTNodeResult::InProgress;
}

void UBTTaskJumps::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// Get the companion that uses this bb
	const AAIController* AIController = OwnerComp.GetAIOwner();
	ACompanion* companion = Cast<ACompanion>(AIController->GetPawn());
	if (!companion) return;

	if (companion->JumpForceTimeRemaining <= 0.0f)
	{
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

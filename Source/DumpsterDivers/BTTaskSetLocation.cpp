// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskSetLocation.h"
#include "AIController.h"
#include "Companion.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTaskSetLocation::UBTTaskSetLocation()
{
	NodeName = "SetLocation";
	//bNotifyTick = true;
}
EBTNodeResult::Type UBTTaskSetLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Get the AI Controller
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	
	// Get the companion that uses this bb
	ACompanion* companion = Cast<ACompanion>(AIController->GetPawn());
	if (!companion) return EBTNodeResult::Failed;

	// // Get the player object
	auto blackboard = OwnerComp.GetBlackboardComponent();
	if (!blackboard) return EBTNodeResult::Failed;
	
	//AActor* raccoon = Cast<AActor>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	//if (!raccoon) return EBTNodeResult::Failed;
	AActor* PlayerActor = Cast<AActor>(blackboard->GetValueAsObject(RaccoonKey.SelectedKeyName));
	if (!PlayerActor) return EBTNodeResult::Failed;

	float distanceToPlayer = FVector::Dist(companion->GetActorLocation(), PlayerActor->GetActorLocation());
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("distance %f"), distanceToPlayer));


	FVector endRelativePos = FVector(PlayerActor->GetActorForwardVector().X + 200.0f,
										PlayerActor->GetActorForwardVector().Y + 200.0f,
										PlayerActor->GetActorForwardVector().Z); 
	companion->SetActorLocation(PlayerActor->GetActorLocation() + endRelativePos);
	
	
	return EBTNodeResult::Succeeded;
}

// void UBTTaskSetLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
// {
// 	// Get the companion that uses this bb
// 	const AAIController* AIController = OwnerComp.GetAIOwner();
// 	ACompanion* companion = Cast<ACompanion>(AIController->GetPawn());
// 	if (!companion) return;
//
// 	if (companion->JumpForceTimeRemaining <= 0.0f)
// 	{
// 		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
// 	}
// }
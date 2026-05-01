// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskFindPlayer.h"

#include "AIController.h"
#include "Companion.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTaskFindPlayer::UBTTaskFindPlayer()
{
	NodeName = "Find Player";
}



EBTNodeResult::Type UBTTaskFindPlayer::ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory)
{

	if (APawn* raccoon = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		// Get the AI Controller
		const AAIController* AIController = Comp.GetAIOwner();
		if (!AIController) return EBTNodeResult::Failed;
		// Get the Enemy Pawn/Character
		APawn* AiCharacter = AIController->GetPawn();
		if (!AiCharacter) return EBTNodeResult::Failed;
		auto blackboard = Comp.GetBlackboardComponent();

		float distanceToPlayer = FVector::Dist(AiCharacter->GetActorLocation(), raccoon->GetActorLocation());
		
		if (blackboard)
		{
			blackboard->SetValueAsObject(RaccoonKey.SelectedKeyName, raccoon);
		}
		if (ACompanion* Companion = Cast<ACompanion>(AiCharacter))
		{
			if (blackboard) blackboard->SetValueAsBool(InRangeKey.SelectedKeyName, (distanceToPlayer <= 350.f));
		}

		if (distanceToPlayer > ViewDistance) return EBTNodeResult::Failed;

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Suck.h"
#include "AIController.h"
#include "SuckEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Suck::UBTT_Suck(): PlayerActor(nullptr), EnemyActor(nullptr), BTComp(nullptr), BlackboardComp(nullptr)
{
	NodeName = "Suck";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_Suck::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTTaskSuckMemory* Memory = reinterpret_cast<FBTTaskSuckMemory*>(NodeMemory);
	Memory->BTComp = &OwnerComp;
	
	// Get the AI Controller
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
   
	// Get the Enemy Pawn/Character
	APawn* Enemy = AIController->GetPawn();
	if (!Enemy) return EBTNodeResult::Failed;
   
	// Cast to your specific enemy class
	Memory->EnemyActor = Cast<ASuckEnemy>(Enemy);
	if (!Memory->EnemyActor) return EBTNodeResult::Failed;

	// Get player location from blackboard
	Memory->BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!Memory->BlackboardComp) return EBTNodeResult::Failed;
    
	Memory->PlayerActor = Cast<AActor>(Memory->BlackboardComp->GetValueAsObject(RaccoonKey.SelectedKeyName));
	if (!Memory->PlayerActor) return EBTNodeResult::Failed;

	if (Memory->EnemyActor)
	{
		// Set the attack end time in the blackboard
		const float newAttackEndTime = GetWorld()->GetTimeSeconds() + ChargeTime;
		Memory->BlackboardComp->SetValueAsFloat(AttackEndTime.SelectedKeyName, newAttackEndTime);
            
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTT_Suck::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTTaskSuckMemory* Memory = reinterpret_cast<FBTTaskSuckMemory*>(NodeMemory);

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !Memory->EnemyActor || !Memory->PlayerActor || !Memory->BlackboardComp) 
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* Enemy = AIController->GetPawn();
	if (!Enemy)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	Telegraph(Memory, Enemy, DeltaSeconds);
   
    
	// Check if the charge is complete
	if (GetWorld()->GetTimeSeconds() >= Memory->BlackboardComp->GetValueAsFloat(AttackEndTime.SelectedKeyName))
	{
		Memory->telegraphflag = false;
		FRotator CurrentRotation = Enemy->GetActorRotation();
		FRotator NewRotation = CurrentRotation;
		NewRotation.Pitch = -20.0f;
		Enemy->SetActorRotation(NewRotation);
        
		OnChargeComplete(&OwnerComp, Memory);
	}
}

void UBTT_Suck::OnChargeComplete(UBehaviorTreeComponent* OwnerComp, FBTTaskSuckMemory* Memory) const
{
	if (!OwnerComp) return;
    
	if (AAIController* AIController = OwnerComp->GetAIOwner())
	{
		if (AEnemy* EnemyCharacter = Cast<ASuckEnemy>(AIController->GetPawn()))
		{
			EnemyCharacter->Attack();
			FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

void UBTT_Suck::Telegraph(FBTTaskSuckMemory* Memory, APawn* Enemy, float DeltaSeconds) const
{
	float time_in_telegraph = Memory->BlackboardComp->GetValueAsFloat(AttackEndTime.SelectedKeyName) - GetWorld()->GetTimeSeconds();
	float flash_time = 0.6f; //edit for flash duration
	// Tilt the enemy back first
	float TiltAngle = 80.0f; // Adjust this value as needed
	FRotator CurrentRotation = Enemy->GetActorRotation();
	FRotator TargetRotation = CurrentRotation;
	TargetRotation.Pitch += TiltAngle; // Add the tilt angle to the pitch

	// Interpolate smoothly towards the target rotation for tilting
	float TiltInterpSpeed = 2.0f; // Speed of interpolation for tilting
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, TiltInterpSpeed);
	Enemy->SetActorRotation(NewRotation);

	// Calculate desired horizontal rotation
	FVector DirectionToPlayer = (Memory->PlayerActor->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
	CurrentRotation = Enemy->GetActorRotation();
	TargetRotation = DirectionToPlayer.Rotation();

	// Interpolate horizontal rotation
	const float RotationSpeed = 5.0f; // Adjust this value to control rotation speed
	NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);
	Enemy->SetActorRotation(NewRotation);

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("tit:%f"),time_in_telegraph));

	if (time_in_telegraph < flash_time && !Memory->telegraphflag)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("tit:%f"),time_in_telegraph));
		Cast<ASuckEnemy>(Enemy)->Start_telegraph_flash(flash_time);
		Memory->telegraphflag = true;
		Cast<ASuckEnemy>(Enemy)->Start_Suck(flash_time);
	}
}
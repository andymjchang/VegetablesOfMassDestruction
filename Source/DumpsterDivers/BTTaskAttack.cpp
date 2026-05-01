#include "BTTaskAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "AIController.h"
#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"

UBTTaskAttack::UBTTaskAttack(): PlayerActor(nullptr), EnemyActor(nullptr), BTComp(nullptr), BlackboardComp(nullptr)
{
    NodeName = "Attack";
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FBTTaskAttackMemory* Memory = reinterpret_cast<FBTTaskAttackMemory*>(NodeMemory);
    Memory->BTComp = &OwnerComp;
    
    // Get the AI Controller
    const AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
   
    // Get the Enemy Pawn/Character
    APawn* Enemy = AIController->GetPawn();
    if (!Enemy) return EBTNodeResult::Failed;
   
    // Cast to your specific enemy class
    Memory->EnemyActor = Cast<AEnemy>(Enemy);
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

void UBTTaskAttack::OnChargeComplete(UBehaviorTreeComponent* OwnerComp, FBTTaskAttackMemory* Memory) const
{
    if (!OwnerComp) return;
    
    if (AAIController* AIController = OwnerComp->GetAIOwner())
    {
        if (AEnemy* EnemyCharacter = Cast<AEnemy>(AIController->GetPawn()))
        {
            EnemyCharacter->Attack();
            FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        }
    }
}

void UBTTaskAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FBTTaskAttackMemory* Memory = reinterpret_cast<FBTTaskAttackMemory*>(NodeMemory);

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
        Cast<AEnemy>(Enemy)->endTelegraph();
        
        OnChargeComplete(&OwnerComp, Memory);
    }
}

void UBTTaskAttack::Telegraph(FBTTaskAttackMemory* Memory, APawn* Enemy, float DeltaSeconds)
{
    float time_in_telegraph = Memory->BlackboardComp->GetValueAsFloat(AttackEndTime.SelectedKeyName) - GetWorld()->GetTimeSeconds();
    float flash_time = ChargeTime;
    Cast<AEnemy>(Enemy)->Telegraph(DeltaSeconds);

    if (time_in_telegraph < flash_time && !Memory->telegraphflag)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("tit:%f"),time_in_telegraph));
        Cast<AEnemy>(Enemy)->Start_telegraph_flash(flash_time);
        Memory->telegraphflag = true;
    }
}
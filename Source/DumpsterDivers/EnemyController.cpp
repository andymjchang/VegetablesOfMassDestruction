// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"


AEnemyController::AEnemyController() {
	BehaviorComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent")); 
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent")); 
	

}

void AEnemyController::OnPossess(APawn * InPawn) {
	Super::OnPossess(InPawn); 
    
	AEnemy * AICharacter = Cast<AEnemy>(InPawn); 
	if (AICharacter) {

		if (AICharacter->BehaviorTree) {

			if (AICharacter->BehaviorTree->BlackboardAsset) {

				BlackboardComponent->InitializeBlackboard(*(AICharacter->BehaviorTree->BlackboardAsset));
				// ARaccoon* raccoon = Cast<ARaccoon>(UGameplayStatics::GetPlayerCharacter(this, 0));
				//
				//
				// BlackboardComponent->SetValueAsObject("AttackTarget", raccoon); 
				BehaviorComponent->StartTree(*AICharacter->BehaviorTree);
			}
		}
	}
}

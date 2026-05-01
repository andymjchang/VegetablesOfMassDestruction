// Fill out your copyright notice in the Description page of Project Settings.
#include "CompanionController.h"

#include "Companion.h"

#include "BehaviorTree/BehaviorTree.h"

ACompanionController::ACompanionController() {
	BehaviorComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent")); 
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent")); 
	

}

void ACompanionController::OnPossess(APawn * InPawn) {
	Super::OnPossess(InPawn); 
    
	ACompanion * AICharacter = Cast<ACompanion>(InPawn); 
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
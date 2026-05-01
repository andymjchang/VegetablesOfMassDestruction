// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineComponent.h"

#include "Raccoon.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentState = EPlayerState::RIDLE;
}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UStateMachineComponent::OnEnterState(EPlayerState NewState)
{
	ARaccoon* player = Cast<ARaccoon>(GetOwner());

	TArray<EPlayerState>& inputBuffer = player->inputBuffer;
	EPlayerState state;
	while (!inputBuffer.IsEmpty())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("enter2: %d"), inputBuffer.Num()));
		state = inputBuffer.Pop();
		if (CanTransitionTo(state))
		{
			player->handleBufferedInput(state,NewState);
			return false;
		}
	}
	return true;
}

void UStateMachineComponent::OnExitState(EPlayerState OldState)
{
	ARaccoon* player = Cast<ARaccoon>(GetOwner());

	TArray<EPlayerState>& inputBuffer = player->inputBuffer;
	
	switch (OldState)
	{
	case EPlayerState::EATING:
		if (!player) return;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		player->CustomTimeDilation = 1.0f;
		player->StopZoom();
		player->GetAnimationSpline()->StopSplineMovement();
		break;

	case EPlayerState::RECOVERY:
		if (!player) return;
		player->GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
		break;

	case EPlayerState::ATTACKING:
		if (!player) return;
		player->DisableAttackHitbox();
		break;

	case EPlayerState::PIG:
		if (!player) return;
		player->GetCharacterMovement()->bOrientRotationToMovement = true;
		player->GetCharacterMovement()->MaxWalkSpeed = player->BaseMaxWalkSpeed;
		
	default:
		break;
	}
}


// Called every frame
void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UStateMachineComponent::ChangeState(EPlayerState NewState)
{
	ARaccoon* player = Cast<ARaccoon>(GetOwner());
	if (!player) return false;
	
	TArray<EPlayerState>& inputBuffer = player->inputBuffer;

	if (CanTransitionTo(NewState))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("CurrentState: %d"), CurrentState));
		if (CurrentState == EPlayerState::RECOVERY && !(NewState == EPlayerState::ATTACKING))
		{
			player->combo = 0;
		}
		OnExitState(CurrentState);
		CurrentState = NewState;
		bool goToBuffer = OnEnterState(NewState);
		OnStateChangedDelegate.Broadcast(NewState);
		
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("NewState: %d"), NewState));
		return goToBuffer;
	}
	else
	{
		player->addToBuffer(NewState);
	}
	return false;
}


bool UStateMachineComponent::CanTransitionTo(EPlayerState NewState)
{
	
	return !StateTransitionMap.IsEmpty() && StateTransitionMap[CurrentState].Contains(NewState);
}

EPlayerState UStateMachineComponent::GetState()
{
	return CurrentState;
}

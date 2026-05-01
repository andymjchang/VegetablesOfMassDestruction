// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachineComponent.generated.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8 
{
	RIDLE		UMETA(DisplayName = "Running/Idle"),
	DASHING		UMETA(DisplayName = "Dashing"),
	ATTACKING	UMETA(DisplayName = "Attacking"),
	RECOVERY	UMETA(DisplayName = "Recovery"),
	EATING		UMETA(DisplayName = "Eating"),
	HITSTUN		UMETA(DisplayName = "Hit Stun"),
	DEATH		UMETA(DisplayName = "Death"),
	PIG			UMETA(DisplayName = "Pig"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EPlayerState, NewState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUMPSTERDIVERS_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	EPlayerState CurrentState;

	/*
	Determines what states can be transitioned to from a certain states.

	Transition map is only relevant for inputs which attempt to change or 'interrupt' the current state (i.e. DASHING).
	States that are committal actions (i.e. ATTACKING, EATING, HITSTUN, etc.) will set the state to RIDLE after completion.
	
	TBD: When hit with lethal attack, should player go to HITSTUN then DEATH state or straight to DEATH state?
	*/
	using S = EPlayerState;
	TMap<EPlayerState, TSet<EPlayerState>> StateTransitionMap = {
		{S::RIDLE, 	{S::DASHING, S::ATTACKING, S::EATING, S::HITSTUN, S::DEATH, S::PIG}},
		{S::DASHING, 	{S::RIDLE, S::HITSTUN, S::DEATH, S::PIG}},
		{S::ATTACKING,	{S::RECOVERY, S::HITSTUN, S::DEATH}},
		{S::RECOVERY,	{S::ATTACKING, S::RIDLE, S::DASHING, S::EATING, S::HITSTUN, S::DEATH}},
		{S::EATING, 	{S::RIDLE, S::DASHING, S::HITSTUN, S::DEATH}},
		{S::HITSTUN, 	{S::RIDLE, S::DEATH}},
		{S::DEATH, 	{}},
		{S::PIG, 	{S::RIDLE, S::HITSTUN, S::DEATH}},
	};


public:	
	// Sets default values for this component's properties
	UStateMachineComponent();

	UPROPERTY(BlueprintAssignable)
	FOnStateChanged OnStateChangedDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool OnEnterState(EPlayerState NewState);
	void OnExitState(EPlayerState OldState);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool ChangeState(EPlayerState NewState);

	UFUNCTION(BlueprintCallable)
	bool CanTransitionTo(EPlayerState NewState);

	UFUNCTION(BlueprintCallable)
	EPlayerState GetState();
};

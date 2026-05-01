// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Raccoon.h"
#include "RaccoonGUI.h"
#include "RaccoonControllerInterface.h"
#include "GameManagerInstance.h"

#include "RaccoonController.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API ARaccoonController : public APlayerController
{
	GENERATED_BODY()

public:
	ARaccoonController();

	UPROPERTY()
	URaccoonGUI* RaccoonGUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
	class UInputMappingContext* DefaultMappingContext;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
		class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
		class UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
		class UInputAction* InventoryScrollAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
		class UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
		class UInputAction* EatAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
	class UInputAction* RestartLevelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
	class UInputAction* RestartGameAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = " true"))
	class UInputAction* SwitchCompanionAction;
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void OnMovePressed(const FInputActionValue& Value);
	void OnAttackPressed(const FInputActionValue& Value);
	void OnDashPressed(const FInputActionValue& Value);
	void OnEatPressed(const FInputActionValue& Value);
	void OnInventoryScrollPressed(const FInputActionValue& Value);
	void OnSwitchCompanionPressed(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRestartLevelPressed(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent)
	void OnRestartGamePressed(const FInputActionValue& Value);
};



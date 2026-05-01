// Fill out your copyright notice in the Description page of Project Settings.


#include "RaccoonController.h"

ARaccoonController::ARaccoonController()
{
}

void ARaccoonController::BeginPlay() {
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}


void ARaccoonController::SetupInputComponent() {
	Super::SetupInputComponent();


	if (UEnhancedInputComponent* EnhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(InputComponent)) {

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered,
			this, &ARaccoonController::OnMovePressed);

        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered,
			this, &ARaccoonController::OnAttackPressed);

		EnhancedInputComponent->BindAction(EatAction, ETriggerEvent::Triggered,
			this, &ARaccoonController::OnEatPressed);

		EnhancedInputComponent->BindAction(InventoryScrollAction, ETriggerEvent::Triggered,
			this, &ARaccoonController::OnInventoryScrollPressed);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started,
			this, &ARaccoonController::OnDashPressed);
		
		EnhancedInputComponent->BindAction(RestartLevelAction, ETriggerEvent::Started,
			this, &ARaccoonController::OnRestartLevelPressed);

		EnhancedInputComponent->BindAction(RestartGameAction, ETriggerEvent::Started,
			this, &ARaccoonController::OnRestartGamePressed);

		EnhancedInputComponent->BindAction(SwitchCompanionAction, ETriggerEvent::Started,
			this, &ARaccoonController::OnSwitchCompanionPressed);
	}
}

void ARaccoonController::OnMovePressed(const FInputActionValue& Value) {
	FVector2D MovementVector = Value.Get<FVector2D>(); 
    ARaccoon* character = Cast<ARaccoon>(this->GetCharacter());
	if (character)
	{
		character->MoveEvent(MovementVector);
	}
}


void ARaccoonController::OnAttackPressed(const FInputActionValue& Value) {    
    ARaccoon* character = Cast<ARaccoon>(this->GetCharacter());
	if (character)
	{
		//character->inputBuffer.Push(EPlayerState::ATTACKING);
		//character->handleBufferedInput(EPlayerState::ATTACKING);
		character->AttackEvent();
	}
}

void ARaccoonController::OnDashPressed(const FInputActionValue& Value) {
	FVector2D MovementVector = Value.Get<FVector2D>(); 
	ARaccoon* character = Cast<ARaccoon>(this->GetCharacter());
	if (character)
	{
		//character->DashEvent(MovementVector);
		character->DashEvent();
	}
}

void ARaccoonController::OnEatPressed(const FInputActionValue& Value)
{
	ARaccoon* character = Cast<ARaccoon>(this->GetCharacter());
	if (character)
	{
		character->EatEvent();
	}
}

void ARaccoonController::OnInventoryScrollPressed(const FInputActionValue& Value)
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
	if (gameInstance) {
		UInventorySubsystem* inventory = gameInstance->GetSubsystem<UInventorySubsystem>(); 
		if (inventory) {
			inventory->InventoryScrollEvent(Value); 
		}
	}
}

void ARaccoonController::OnSwitchCompanionPressed(const FInputActionValue& Value)
{
	ARaccoon* character = Cast<ARaccoon>(this->GetCharacter());
	if (character)
	{
		character->SwitchCompanionEvent();
	}
}



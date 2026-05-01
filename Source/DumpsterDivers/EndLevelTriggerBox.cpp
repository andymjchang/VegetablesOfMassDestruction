// Fill out your copyright notice in the Description page of Project Settings.


#include "EndLevelTriggerBox.h"

#include "CustomGameViewportClient.h"
#include "Raccoon.h"
#include "RaccoonController.h"
#include "SaveDataSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEndLevelTriggerBox::AEndLevelTriggerBox()
{
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = true;
	NextLevel = nullptr;
	FadeOutDuration = 1.f;
	AutoWalkSpeed = 0.5f;
	Player = nullptr;
	AutoWalking = false;
	MovementDirection = FVector(0, 0, 0);
}

void AEndLevelTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AEndLevelTriggerBox::OnBeginOverlap);
	
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Begin Play"));

}

void AEndLevelTriggerBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(OpenNextLevelTimer);
}

void AEndLevelTriggerBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (AutoWalking && IsValid(Player)) Player->AddMovementInput(MovementDirection, AutoWalkSpeed);
}

void AEndLevelTriggerBox::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!IsValid(OtherActor) || !OtherActor->IsA(ARaccoon::StaticClass())) return;

	USaveDataSubsystem* SaveDataSubsystem = GetGameInstance()->GetSubsystem<USaveDataSubsystem>();
	if (IsValid(SaveDataSubsystem)) SaveDataSubsystem->SaveData();

	UCustomGameViewportClient* Viewport = Cast<UCustomGameViewportClient>(GetWorld()->GetGameViewport());
	if (IsValid(Viewport)) Viewport->Fade(FadeOutDuration, true);

	WalkTowardsCenter(OtherActor);

	GetWorld()->GetTimerManager().SetTimer(
		OpenNextLevelTimer,
		[this]()->void
		{
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, NextLevel);
		},
		FadeOutDuration,
		false);
}

void AEndLevelTriggerBox::WalkTowardsCenter(AActor* Actor)
{
	if (!IsValid(Actor)) return;

	Player = Cast<ARaccoon>(Actor);
	if (!IsValid(Player)) return;

	ARaccoonController* Controller = Cast<ARaccoonController>(Player->GetController());
	if (!IsValid(Controller)) return;
	
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Controller->GetLocalPlayer());
	if (EnhancedInputSubsystem) EnhancedInputSubsystem->RemoveMappingContext(Controller->DefaultMappingContext);

	MovementDirection = ((GetActorLocation() - Player->GetActorLocation()) * FVector(1, 1, 0)).GetSafeNormal();
	AutoWalking = true;
}

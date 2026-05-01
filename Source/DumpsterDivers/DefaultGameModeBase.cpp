// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameModeBase.h"

#include "CustomGameViewportClient.h"
#include "GUISubsystem.h"
#include "UObject/ConstructorHelpers.h"
#include "InventorySubsystem.h"
#include "SaveDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSectionsSystem/LevelManagerSubsystem.h"

ADefaultGameModeBase::ADefaultGameModeBase() {

	// set default pawn class to our Blueprinted character
	//Assumes the BP Pawn is stored in a folder under Content called "Blueprints" and that it's named BP_WorldPawn
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_Raccoon"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprint/BP_RaccoonController"));
	if (PlayerControllerBPClass.Class)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	FadeInDuration = 1.f;
}

void ADefaultGameModeBase::BeginPlay() {
	Super::BeginPlay();
	
	UInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UInventorySubsystem>();
	if (IsValid(InventorySubsystem)) InventorySubsystem->InitInventory(weaponTable, tagVFXTable);

	UGUISubsystem* GUISubsystem = GetGameInstance()->GetSubsystem<UGUISubsystem>();
	if (IsValid(GUISubsystem)) GUISubsystem->InitializeGUIs(EnemyIndicatorUIClass);
    
	ULevelManagerSubsystem* LevelManager = GetWorld()->GetSubsystem<ULevelManagerSubsystem>();
	if (IsValid(LevelManager)) LevelManager->InitLevelManager();

	USaveDataSubsystem* SaveDataSubsystem = GetGameInstance()->GetSubsystem<USaveDataSubsystem>();
	if (IsValid(SaveDataSubsystem)) SaveDataSubsystem->LoadData();

	UGameManagerInstance* GameManager = Cast<UGameManagerInstance>(GetGameInstance());
	if (IsValid(GameManager)) GameManager->EnableLoadingScreens();

	UCustomGameViewportClient* Viewport = Cast<UCustomGameViewportClient>(GetWorld()->GetGameViewport());
	if (IsValid(Viewport)) Viewport->Fade(FadeInDuration, false);
}

void ADefaultGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

  
}



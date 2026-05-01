// Fill out your copyright notice in the Description page of Project Settings.


#include "CompanionCage.h"

#include "AugmentGUI.h"
#include "GameManagerInstance.h"
#include "GUISubsystem.h"
#include "Raccoon.h"

// Sets default values
ACompanionCage::ACompanionCage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACompanionCage::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACompanionCage::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);

}

void ACompanionCage::OnBreak()
{
	// Access Subsystems
	if (const UGameManagerInstance* GameInstance = Cast<UGameManagerInstance>(GetGameInstance()))
	{
		if (UGUISubsystem* GUI = GameInstance->GetSubsystem<UGUISubsystem>())
		{
			if (UAugmentGUI* AugmentGUI = GUI->AugmentGUI)
			{
				AugmentGUI->SetVisible(true);
				AugmentGUI->PlayFadeAnimation(true);
			}
		}
	}
}
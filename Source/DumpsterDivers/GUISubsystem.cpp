// Fill out your copyright notice in the Description page of Project Settings.


#include "GUISubsystem.h"

void UGUISubsystem::InitializeGUIs(TSubclassOf<UEnemyIndicatorUI> EnemyIndicatorUIClass)
{
	UEnemyIndicatorUI* NewEnemyIndicatorUI = CreateWidget<UEnemyIndicatorUI>(GetWorld(), EnemyIndicatorUIClass);
	if (!IsValid(NewEnemyIndicatorUI)) return;
	
	NewEnemyIndicatorUI->AddToViewport(999);
	EnemyIndicatorUI = NewEnemyIndicatorUI;
}

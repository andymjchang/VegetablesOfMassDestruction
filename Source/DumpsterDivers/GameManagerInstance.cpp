// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManagerInstance.h"
// #include "MoviePlayer.h"
#include "MoviePlayer.h"
#include "Blueprint/UserWidget.h"


UGameManagerInstance::UGameManagerInstance()
{
	WidgetClass = nullptr;
	MinimumDisplayTime = 2.f;
	LoadingScreensEnabled = false;
}

void UGameManagerInstance::Init()
{
	Super::Init();
}

void UGameManagerInstance::BeginLoadingScreen(const FString& MapName)
{
	if (!IsValid(WidgetClass) || IsRunningDedicatedServer()) return;
	
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	LoadingScreen.MinimumLoadingScreenDisplayTime = MinimumDisplayTime;
	LoadingScreen.bMoviesAreSkippable = false;
	
	UUserWidget* LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	LoadingScreen.WidgetLoadingScreen = LoadingScreenWidget->TakeWidget();
 
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UGameManagerInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	
}

void UGameManagerInstance::EnableLoadingScreens()
{
	if (!LoadingScreensEnabled)
	{
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UGameManagerInstance::BeginLoadingScreen);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGameManagerInstance::EndLoadingScreen);
		LoadingScreensEnabled = true;
	}
}



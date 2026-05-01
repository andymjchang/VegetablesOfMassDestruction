// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameManagerInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UGameManagerInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UGameManagerInstance();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinimumDisplayTime;
	
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	virtual void BeginLoadingScreen(const FString& MapName);

	UFUNCTION(BlueprintCallable)
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

	UFUNCTION(BlueprintCallable)
	void EnableLoadingScreens();

private:
	bool LoadingScreensEnabled;
};

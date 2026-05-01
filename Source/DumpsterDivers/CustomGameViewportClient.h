// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "CustomGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UCustomGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
	
public:
	UCustomGameViewportClient();
	
	virtual void PostRender(UCanvas* Canvas) override;

	virtual void ClearFade();

	virtual void Fade(const float Duration, const bool ToBlack);

	void DrawScreenFade(UCanvas* Canvas);

private:
	bool bFading;
	bool bToBlack;
	float FadeAlpha;
	float FadeStartTime;
	float FadeDuration;
};

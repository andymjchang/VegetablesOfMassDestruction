// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "EnemyIndicatorUI.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UEnemyIndicatorUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IndicatorTexture;

	/**
	 * If UseRadialIndicator is true, IconScreenOffset will be used as the indicator's offset from the center.
	 * Otherwise, IconScreenOffset is used as the indicator's offset from the edge of the screen.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IconScreenOffset = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseRadialIndicator = false;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void InitializeSectionIndicators(int SectionID);

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

private:
	TArray<AEnemy*>* Enemies;

	UPROPERTY()
	TMap<AEnemy*, UImage*> Indicators;

	void UpdateIndicatorsMap();
	void UpdateIndicatorsPosition();
	FVector2D GetEdgePositionFromCenter(const FVector2D& Direction, const FVector2D& ViewportCenter,const FVector2D& ViewportSize);
};

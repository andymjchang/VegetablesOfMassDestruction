// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyIndicatorUI.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSectionsSystem/LevelManagerSubsystem.h"

void UEnemyIndicatorUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEnemyIndicatorUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateIndicatorsMap();
	UpdateIndicatorsPosition();
}

void UEnemyIndicatorUI::InitializeSectionIndicators(int SectionID)
{
	ULevelManagerSubsystem* LevelManager = GetWorld()->GetSubsystem<ULevelManagerSubsystem>();
	if (!IsValid(LevelManager)) return;
	
	Enemies = LevelManager->GetEnemiesInSection(SectionID);
}

void UEnemyIndicatorUI::UpdateIndicatorsMap()
{
	if (!Enemies) return;

	// Create an indicator for newly spawned enemies
	for (AEnemy* Enemy : *Enemies)
	{
		if (Indicators.Contains(Enemy)) continue;

		// Create new indicator instance and add to map
		UImage* Icon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		if (!IsValid(Icon) || !IsValid(IndicatorTexture) || !IsValid(CanvasPanel)) continue;

		Icon->SetBrushFromTexture(IndicatorTexture, true);
		CanvasPanel->AddChildToCanvas(Icon);

		// Set canvas slot parameters
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Icon->Slot);
		if (!IsValid(CanvasSlot)) continue;

		CanvasSlot->SetPosition(FVector2D::ZeroVector);
		CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		CanvasSlot->SetSize(FVector2D(IndicatorTexture->GetSizeX(), IndicatorTexture->GetSizeY()) * 0.1f);
		
		Indicators.Add(Enemy, Icon);
	}
	
	// Remove indicators for enemies that no longer exist
	TMap<AEnemy*, UImage*> ReducedIndicators = Indicators;
	for (TPair<AEnemy*, UImage*> Indicator : Indicators)
	{
		if (IsValid(Indicator.Key)) continue;
	
		Indicator.Value->RemoveFromParent();
		ReducedIndicators.Remove(Indicator.Key);
	}
	Indicators = ReducedIndicators;
}

void UEnemyIndicatorUI::UpdateIndicatorsPosition()
{
	APlayerController* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(Controller)) return;

	// Get viewport size
	int ViewportSizeX;
	int ViewportSizeY;
	Controller->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Adjust for scale
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(Controller);
	ViewportSizeX /= ViewportScale;
	ViewportSizeY /= ViewportScale;
	FVector2D ViewportCenter = FVector2D(ViewportSizeX / 2.f, ViewportSizeY / 2.f);
	
	for (TPair<AEnemy*, UImage*> Indicator : Indicators)
	{
		// Calculate enemy position on screen space
		FVector2D AbsoluteScreenLocation;
		Controller->ProjectWorldLocationToScreen(Indicator.Key->GetActorLocation(), AbsoluteScreenLocation);
		FVector2D RelativePosition = AbsoluteScreenLocation / ViewportScale;

		// Only show indicator if enemy is offscreen
		if (RelativePosition.X > ViewportSizeX || RelativePosition.Y > ViewportSizeY ||
			RelativePosition.X < 0.0f || RelativePosition.Y < 0.0f)
		{
			Indicator.Value->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Indicator.Value->SetVisibility(ESlateVisibility::Hidden);
		}

		// Calculate new position & angle of icon
		FVector2D Direction = (RelativePosition - ViewportCenter).GetSafeNormal();
		FVector2D NewPosition;
		if (UseRadialIndicator)
		{
			NewPosition = ViewportCenter + Direction * IconScreenOffset;
		}
		else
		{
			NewPosition = GetEdgePositionFromCenter(Direction, ViewportCenter, FVector2D(ViewportSizeX, ViewportSizeY));
			NewPosition.X = FMath::Clamp(NewPosition.X, IconScreenOffset, ViewportSizeX - IconScreenOffset);
			NewPosition.Y = FMath::Clamp(NewPosition.Y, IconScreenOffset, ViewportSizeY - IconScreenOffset);
		}
		
		float AngleRadians = FMath::Atan2(Direction.Y, Direction.X);
		float AngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 90.f;

		// Set indicator location
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Indicator.Value->Slot);
		if (!IsValid(CanvasSlot)) continue;
		CanvasSlot->SetPosition(NewPosition);
		Indicator.Value->SetRenderTransformAngle(AngleDegrees);
	}
}

FVector2D UEnemyIndicatorUI::GetEdgePositionFromCenter(const FVector2D& Direction, const FVector2D& ViewportCenter, const FVector2D& ViewportSize)
{
	FVector2D HalfSize = ViewportSize * 0.5f;

	float dx = Direction.X;
	float dy = Direction.Y;
	
	float scaleX = (dx != 0.0f) ? HalfSize.X / FMath::Abs(dx) : FLT_MAX;
	float scaleY = (dy != 0.0f) ? HalfSize.Y / FMath::Abs(dy) : FLT_MAX;

	float Scale = FMath::Min(scaleX, scaleY);
	return ViewportCenter + Direction * Scale;
}

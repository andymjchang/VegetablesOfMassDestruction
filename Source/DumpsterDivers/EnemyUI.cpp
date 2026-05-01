// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyUI.h"

void UEnemyUI::UpdateHealthBar(float healthPercentage)
{
    UProgressBar* HealthBarWidget = Cast<UProgressBar>(GetWidgetFromName(HealthBarName));
    if (!HealthBarWidget) return;
    HealthBarWidget->SetPercent(healthPercentage);
}

void UEnemyUI::UpdateArmorBar(float ArmorPercentage)
{
    UProgressBar* ArmorBarWidget = Cast<UProgressBar>(GetWidgetFromName(ArmorBarName));
    if (!ArmorBarWidget) return;
    ArmorBarWidget->SetPercent(ArmorPercentage);
}

void UEnemyUI::ShowArmorBar()
{
    UProgressBar* ArmorBarWidget = Cast<UProgressBar>(GetWidgetFromName(ArmorBarName));
    if (!ArmorBarWidget) return;
    ArmorBarWidget->SetVisibility(ESlateVisibility::Visible);
}

void UEnemyUI::HideArmorBar()
{
    UProgressBar* ArmorBarWidget = Cast<UProgressBar>(GetWidgetFromName(ArmorBarName));
    if (!ArmorBarWidget) return;
    ArmorBarWidget->SetVisibility(ESlateVisibility::Hidden);
}

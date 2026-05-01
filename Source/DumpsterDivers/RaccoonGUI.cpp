// Fill out your copyright notice in the Description page of Project Settings.


#include "RaccoonGUI.h"

#define LOCTEXT_NAMESPACE "HUDSpace"

void URaccoonGUI::NativeConstruct()
{
    Super::NativeConstruct();

    RefreshInventory();
}

void URaccoonGUI::SetInventoryImage(int index, FName id, UTexture2D* icon)
{   
    FString InventoryWidgetName = "InventoryImage" + FString::FromInt(index); 
    UImage* FoundWidget = Cast<UImage>(GetWidgetFromName(FName(InventoryWidgetName)));

    if (FoundWidget) {
        FoundWidget->SetBrushFromTexture(icon);
        FLinearColor tint = (id == "Empty") ? FLinearColor(0, 0, 0, 0) : FLinearColor(1, 1, 1, 1);
        FoundWidget->SetBrushTintColor(tint);
    }
}



void URaccoonGUI::UpdateHighlightInventory(int curIndex, int size) {

    FString InventoryHighlightName = "InventoryBorder" + FString::FromInt(lastIndex); 
    UBorder* HighlightWidget = Cast<UBorder>(GetWidgetFromName(FName(InventoryHighlightName)));

    if (HighlightWidget) {
            HighlightWidget->SetBrushColor(FLinearColor(0, 0, 0, 0)); 
    }


    InventoryHighlightName = "InventoryBorder" + FString::FromInt(curIndex); 
    HighlightWidget = Cast<UBorder>(GetWidgetFromName(FName(InventoryHighlightName)));

    if (HighlightWidget) {
            HighlightWidget->SetBrushColor(FLinearColor(1, 1, 1, 1)); 
    }

    lastIndex = curIndex; 

}

void URaccoonGUI::UpdateHealthBar(float health, float maxHealth)
{
    HealthBar->SetPercent(health / maxHealth);
}

void URaccoonGUI::UpdateManaBar(float CurrentMana, float MaxMana)
{
    ManaBar->SetPercent(CurrentMana / MaxMana);
}

void URaccoonGUI::RefreshInventory()
{
    UInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UInventorySubsystem>();
    if (!InventorySubsystem) return;
    
    TArray<FWeaponStruct>& Inventory = InventorySubsystem->weaponsList;
    UDataTable* WeaponDataTable = InventorySubsystem->weaponTable;
    
    for (int i = 0; i < Inventory.Num(); ++i)
    {
        FName WeaponName = InventorySubsystem->GetItemIDAt(i);
        UTexture2D* WeaponImage = WeaponDataTable->FindRow<FWeaponStruct>(WeaponName, "")->icon;
        SetInventoryImage(i, WeaponName, WeaponImage);
    }
}

void URaccoonGUI::HighlightCompanion(int index)
{
    if (index == 1)
    {
        if (CompanionBorder1) CompanionBorder1->SetBrushColor(FLinearColor(1, 1, 1, 1));
        if (CompanionBorder2) CompanionBorder2->SetBrushColor(FLinearColor(1, 1, 1, 0));
    }
    else
    {
        if (CompanionBorder2) CompanionBorder2->SetBrushColor(FLinearColor(1, 1, 1, 1));
        if (CompanionBorder1) CompanionBorder1->SetBrushColor(FLinearColor(1, 1, 1, 0)); 
    }
}

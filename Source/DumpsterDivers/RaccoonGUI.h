// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "InventorySubsystem.h"

#include "RaccoonGUI.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API URaccoonGUI : public UUserWidget
{
	GENERATED_BODY()

	int lastIndex = 0; 	

public:
	virtual void NativeConstruct() override;
	void SetInventoryImage(int index, FName id, UTexture2D* icon); 
	void UpdateHighlightInventory(int curIndex, int size); 
	void UpdateHealthBar(float health, float maxHealth);
	void UpdateManaBar(float Mana, float MaxMana);

	UFUNCTION(BlueprintCallable)
	void RefreshInventory();
	void HighlightCompanion(int index);

	// Binded Widget Elements
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	UBorder* CompanionBorder1;
	UPROPERTY(meta = (BindWidget))
	UBorder* CompanionBorder2;

	// UPROPERTY(EditDefaultsOnly, Category = "UI")
    // UTexture2D* CarrotImage;

	// UPROPERTY(EditAnywhere, meta = (BindWidget))
	// class UImage* InventoryImage0;
	// UPROPERTY(EditAnywhere, meta = (BindWidget))
	// class UImage* InventoryImage1;
	// UPROPERTY(EditAnywhere, meta = (BindWidget))
	// class UImage* InventoryImage2;
	// UPROPERTY(EditAnywhere, meta = (BindWidget))
	// class UImage* InventoryImage3;
	// // UPROPERTY(EditAnywhere, meta = (BindWidget))
	// // class UImage* InventoryImage4;
};

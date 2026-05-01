// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AugmentGUI.generated.h"

/**
 * 
 */
UCLASS()
class DUMPSTERDIVERS_API UAugmentGUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UAugmentGUI(const FObjectInitializer& ObjectInitializer);
	void TemporaryCompanionSetup();
	void BeginPlay();

	// Binded Widget Elements
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title1;
	UPROPERTY(meta = (BindWidget))
	UImage* Image1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Description1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title2;
	UPROPERTY(meta = (BindWidget))
	UImage* Image2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Description2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title3;
	UPROPERTY(meta = (BindWidget))
	UImage* Image3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Description3;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayFadeAnimation(bool InOut);

	// On click function
	UFUNCTION(BlueprintCallable)
	void OnOption1Clicked();
	UFUNCTION(BlueprintCallable)
	void OnOption2Clicked();
	UFUNCTION(BlueprintCallable)
	void OnOption3Clicked();
	void BaseOptionClicked();

	UFUNCTION(BlueprintCallable)
	void SetVisible(bool Toggle);

	UFUNCTION(BlueprintCallable)
	void RefreshAugmentScreen();
	FAugmentDataStruct* GetRandomAugment(const UDataTable* DataTable);
	void ApplyAugmentEffect(int Index);
	// int32 GetIndexFromRowName(const UDataTable* DataTable, const FName& RowName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AugmentDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
	TSubclassOf<AActor> ChickenClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
	TSubclassOf<AActor> PigClass;

	UPROPERTY(BlueprintReadWrite)
	bool Clickable = false;

private:
	UPROPERTY()
	TArray<FName> SelectedAugments;

	UPROPERTY()
	TArray<int> OptionImplementIndex;
};

USTRUCT(BlueprintType)
struct FAugmentDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RarityLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ImplementIndex;
	
};
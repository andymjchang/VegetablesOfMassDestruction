// Fill out your copyright notice in the Description page of Project Settings.


#include "AugmentGUI.h"

#include "Raccoon.h"
#include "Kismet/KismetMathLibrary.h"

UAugmentGUI::UAugmentGUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OptionImplementIndex.SetNum(3);
	// You can add more initialization logic here if needed.
}

void UAugmentGUI::TemporaryCompanionSetup()
{
	// Get player
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	if (!gameInstance) return;
	ARaccoon* raccoon = Cast<ARaccoon>(gameInstance->GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!raccoon) return;
	raccoon->SpawnCompanion(ChickenClass);
	raccoon->SpawnCompanion(PigClass);
}

void UAugmentGUI::OnOption1Clicked()
{
	if (!Clickable) return;
	BaseOptionClicked();
	ApplyAugmentEffect(OptionImplementIndex[0]);
}

void UAugmentGUI::OnOption2Clicked()
{
	if (!Clickable) return;
	BaseOptionClicked();
	ApplyAugmentEffect(OptionImplementIndex[1]);}

void UAugmentGUI::OnOption3Clicked()
{
	if (!Clickable) return;
	BaseOptionClicked();
	ApplyAugmentEffect(OptionImplementIndex[2]);
}

void UAugmentGUI::BaseOptionClicked()
{
	PlayFadeAnimation(false);
	Clickable = false;
}

void UAugmentGUI::SetVisible(const bool Toggle)
{
	if (Toggle)
	{
		SetVisibility(ESlateVisibility::Visible);
		Clickable = true;
		RefreshAugmentScreen();
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
		Clickable = false;
	}
}

void UAugmentGUI::RefreshAugmentScreen()
{
    // Clear the selected augments array before selecting new ones
    SelectedAugments.Empty();
	
    FAugmentDataStruct* AugmentData = GetRandomAugment(AugmentDataTable);
    if (AugmentData)
    {
        Title1->SetText(AugmentData->Title);
        Image1->SetBrushFromTexture(AugmentData->Image);
        Description1->SetText(AugmentData->Description);
    	OptionImplementIndex[0] = AugmentData->ImplementIndex;
    }

    AugmentData = GetRandomAugment(AugmentDataTable);
    if (AugmentData)
    {
        Title2->SetText(AugmentData->Title);
        Image2->SetBrushFromTexture(AugmentData->Image);
        Description2->SetText(AugmentData->Description);
    	OptionImplementIndex[1] = AugmentData->ImplementIndex;
    }
	
    AugmentData = GetRandomAugment(AugmentDataTable);
    if (AugmentData)
    {
        Title3->SetText(AugmentData->Title);
        Image3->SetBrushFromTexture(AugmentData->Image);
        Description3->SetText(AugmentData->Description);
    	OptionImplementIndex[2] = AugmentData->ImplementIndex;
    }
}

// Add this function to your class
FAugmentDataStruct* UAugmentGUI::GetRandomAugment(const UDataTable* DataTable)
{
    if (!DataTable) return nullptr;

    // Get all row names from the Data Table
    TArray<FName> RowNames = DataTable->GetRowNames();
    if (RowNames.Num() == 0) return nullptr;
    
    // Remove already selected augments from consideration
    TArray<FName> AvailableRowNames;
    for (const FName& RowName : RowNames)
    {
        if (!SelectedAugments.Contains(RowName))
        {
            AvailableRowNames.Add(RowName);
        }
    }
    
    // If all augments have been selected, return nullptr
    if (AvailableRowNames.Num() == 0)
    {
        return nullptr;
    }

    // Pick a random row from available options
    int32 RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, AvailableRowNames.Num() - 1);
    FName RandomRowName = AvailableRowNames[RandomIndex];
    
    // Add to selected list
    SelectedAugments.Add(RandomRowName);

    // Retrieve the data from the Data Table
    static const FString ContextString(TEXT("Random Augment Selection"));
    return DataTable->FindRow<FAugmentDataStruct>(RandomRowName, ContextString);
}

void UAugmentGUI::ApplyAugmentEffect(const int Index)
{
	// Get player
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	if (!gameInstance) return;
	ARaccoon* raccoon = Cast<ARaccoon>(gameInstance->GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!raccoon) return;

	raccoon->CurrentMana = raccoon->MaxMana;
	
	switch (Index)
	{
	case 0:
		raccoon->EggBaseDamage += 10;
		raccoon->SpawnCompanion(ChickenClass);
		break;
	case 1: 
		raccoon->DashCooldown[0] *= 0.75;
		raccoon->SpawnCompanion(ChickenClass);
		break;
	case 2:
		raccoon->EggBaseRadius *= 2;
		raccoon->SpawnCompanion(ChickenClass);
		break;
	default: return;
	}
}

// int32 UAugmentGUI::GetIndexFromRowName(const UDataTable* DataTable, const FName& RowName)
// {
// 	if (!DataTable) return -1;
//
// 	TArray<FName> RowNames = DataTable->GetRowNames();
// 	return RowNames.Find(RowName); // Returns index or INDEX_NONE if not found.
// }
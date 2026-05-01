#include "DT_Tag.h"

// Function to create and populate the data table
UDataTable* UDT_Tag::CreateDT_Tag()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("table made")));
	// Create a new UDataTable instance
	UDataTable* DT_Tags = NewObject<UDataTable>();

	// Set the row struct for the data table (e.g., FTag)
	DT_Tags->RowStruct = FTag::StaticStruct();

	// Create the "spicy" row
	FTag SpicyTag;
	SpicyTag.name = "0";
	SpicyTag.tag_name = "Spicy";
	if (UEffect_Burn* BurnEffect = NewObject<UEffect_Burn>())
	{
		SpicyTag.hit_effect = TScriptInterface<IEffect>(BurnEffect);
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("can't make")));
	}

	// Set the hit and eat effects. Here we assume UEffect_Burn implements IEffect.
	UEffect_Burn* BurnEffect = NewObject<UEffect_Burn>();
	SpicyTag.hit_effect = TScriptInterface<IEffect>(BurnEffect);
	//SpicyTag.eat_effect = TScriptInterface<IEffect>(UEffect_Burn::StaticClass());

	// Add the row to the data table
	DT_Tags->AddRow(SpicyTag.name, SpicyTag);

	// Return the populated data table
	return DT_Tags;
}

UDataTable* UDT_Tag::UpdatewithVFX(UDataTable* tagsTable, UDataTable* tagsVFXTable)
{
	if (!tagsTable) return nullptr;
	if (!tagsVFXTable) return nullptr; 
	// for every item on the table
	// add it to tag table
	TArray<FName> names = tagsVFXTable->GetRowNames();
	for (FName name : names)
	{
		FTag* tag = tagsTable->FindRow<FTag>(name, "");
		FTagVFX* tagVfx = tagsVFXTable->FindRow<FTagVFX>(name, "");
		if (!tag) break;
		if (!tagVfx) break; 
		tag->hit_VFX = tagVfx->HitVFX; 
	}
	return tagsTable; 
}
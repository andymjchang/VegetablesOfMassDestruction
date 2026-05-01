#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Effect.h"
#include "Tag.h"  // Include your Tag struct header
#include "Effect_Burn.h"  // Include Effect_Burn class
#include "DT_Tag.generated.h"

UCLASS()
class DUMPSTERDIVERS_API UDT_Tag : public UObject
{
	GENERATED_BODY()

public:
	// Declare the function to create and populate the data table
	UDataTable* CreateDT_Tag();
	UDataTable* UpdatewithVFX(UDataTable* tagsTable, UDataTable* tagsVFXTable); 

};

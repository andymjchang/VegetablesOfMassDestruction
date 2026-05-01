#pragma once

#include "Effect.h"
#include "Engine/DataTable.h"
#include "Tag.generated.h"

USTRUCT(BlueprintType)
struct DUMPSTERDIVERS_API FTag : public FTableRowBase
{
	GENERATED_BODY()
	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	FName tag_name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	TScriptInterface<IEffect> hit_effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	class UNiagaraSystem* hit_VFX;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	// TScriptInterface<IEffect> eat_effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	FName name;

	// Constructor
	FTag()
		: tag_name(FName(TEXT("Empty")))
		, hit_effect(nullptr)
		, hit_VFX(nullptr)
		//, eat_effect(nullptr)
		, name(FName(TEXT("Empty")))
	{
	}
};

USTRUCT(BlueprintType)
struct DUMPSTERDIVERS_API FTagVFX : public FTableRowBase
{
	GENERATED_BODY()
	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	FName tag_name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	class UNiagaraSystem* HitVFX;

	// Constructor
	FTagVFX()
		: tag_name(FName(TEXT("Empty")))
		, HitVFX(nullptr)
	{
	}
};
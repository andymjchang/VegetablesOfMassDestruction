#pragma once

#include "CoreMinimal.h"
#include "Effect.h"
#include "Effect_Burn.generated.h"

UCLASS()
class DUMPSTERDIVERS_API UEffect_Burn : public UObject, public IEffect
{
    GENERATED_BODY()

public:
    UEffect_Burn();

    virtual void Apply(TScriptInterface<ICombatUnit> IC) override;

private:

    FTimerHandle BurnTimerHandle; // Timer for DoT
    float BurnDamage = 3.0f;      // Damage per tick
    int BurnDuration = 5;    // Total burn duration
};

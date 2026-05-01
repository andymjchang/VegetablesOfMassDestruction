#include "Effect_Burn.h"
#include "TimerManager.h"
#include "CombatUnit.h"

UEffect_Burn::UEffect_Burn()
{
}

void UEffect_Burn::Apply(TScriptInterface<ICombatUnit> IC)
{
	if (IC.GetObject())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("ABOUTTOTICK")));
		IC->TickUnitDamage(BurnDamage, BurnDuration);
	}
}


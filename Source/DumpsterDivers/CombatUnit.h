#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatUnit.generated.h"

UINTERFACE(BlueprintType)
class DUMPSTERDIVERS_API UCombatUnit : public UInterface
{
	GENERATED_BODY()
};

class DUMPSTERDIVERS_API ICombatUnit
{
	GENERATED_BODY()

public:
	virtual void TakeUnitDamage(float Damage) = 0;
	virtual void TickUnitDamage(float Damage, int Duration) = 0;
	//
	// virtual float get_speed() = 0;
	// virtual void set_speed(float spd) = 0;
	//
	//
};

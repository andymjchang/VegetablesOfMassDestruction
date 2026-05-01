#pragma once

#include "CoreMinimal.h"
#include "CombatUnit.h"
#include "UObject/Interface.h"
#include "Effect.generated.h"

// Interface class
UINTERFACE()
class DUMPSTERDIVERS_API UEffect : public UInterface
{ 
	GENERATED_BODY()
};

class DUMPSTERDIVERS_API IEffect
{
	GENERATED_BODY()

public:
	float duration;
	//virtual bool tick();
	// The Apply method will now be a pure virtual function
	virtual void Apply(TScriptInterface<ICombatUnit> IC) = 0;
	virtual void Expire(TScriptInterface<ICombatUnit> IC){}
};

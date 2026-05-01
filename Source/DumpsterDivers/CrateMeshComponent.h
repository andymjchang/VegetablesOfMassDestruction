// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "CrateMeshComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUMPSTERDIVERS_API UCrateMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UCrateMeshComponent();
};

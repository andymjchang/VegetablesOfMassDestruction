// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Decoration.generated.h"

UCLASS()
class DUMPSTERDIVERS_API ADecoration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADecoration();
	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* DestroyVFX;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void OnHit(AActor* OverlappedActor, AActor* OtherActor);
	void SpawnVFX(UNiagaraSystem* VFX) const;
};

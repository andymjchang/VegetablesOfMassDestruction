// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"
#include "Companion.generated.h"

UCLASS()
class DUMPSTERDIVERS_API ACompanion : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACompanion();
	
	UPROPERTY (EditAnywhere) 
	class UBehaviorTree * BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PoofEffectScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem * PoofVFX;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;
	
	
public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// void HopForward();
	
	void SpawnPoofEffect() const;

	void SpawnVFX(UNiagaraSystem* Effect, const FVector& SpawnLocation, const FVector& EffectScale) const;
};

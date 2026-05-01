// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "SuckEnemy.generated.h"

UCLASS()
class DUMPSTERDIVERS_API ASuckEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASuckEnemy();
	UNiagaraComponent* SuckVFXComp;
	FTimerHandle SuckTimer;
	bool isSuck = false;
	bool isShake = false;
	FVector OriginalLocation;
	FRotator OriginalRotation;
	bool bStoredOriginalTransform = false;

	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* SuckVFX;

	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* SmokeVFX;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Start_telegraph_flash(float Duration) override;
	virtual void Telegraph(float DeltaSeconds) override;
	virtual void endTelegraph() override;
	virtual void StartKnockback(const AActor* OtherActor, float StrengthScale = 1) override;
	FVector telegraphScale;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Attack() override;

	void SpawnSuckVFX();

	void EndVFX() const;

	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	void Start_Suck(float duration);

	UPROPERTY(EditAnywhere, Category = "Suck")
	float SuctionRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Suck")
	float SuctionStrength = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Suck")
	float MaxInflate = 1.2f;
	
	UPROPERTY(EditAnywhere, Category = "Suck")
	float InflateSpeed = 0.75f;
	
	void Suck(float DeltaTime);
};

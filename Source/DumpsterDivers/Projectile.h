// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class DUMPSTERDIVERS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere, Category = "Proj")
	float lifeSpan = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Proj")
	float rotationSpeed = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Proj")
	float speed = 750.0f;
	AProjectile();
	FVector direction;
	
	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* DestroyVFX;
	FTimerHandle DestroyTimer;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnVFX(UNiagaraSystem* VFX) const;
	UFUNCTION()
	void OnHit(AActor* OverlappedActor, AActor* OtherActor);
};

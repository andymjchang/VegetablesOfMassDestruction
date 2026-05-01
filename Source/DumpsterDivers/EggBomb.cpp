// Fill out your copyright notice in the Description page of Project Settings.


#include "EggBomb.h"

#include "Components/SphereComponent.h"

// Sets default values
AEggBomb::AEggBomb()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEggBomb::BeginPlay()
{
	Super::BeginPlay();
}

void AEggBomb::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEggBomb::SetHitboxRadius(float RadiusUnscaled) const
{
	USphereComponent* Hitbox = FindComponentByClass<USphereComponent>();
	Hitbox->SetSphereRadius(RadiusUnscaled);
}


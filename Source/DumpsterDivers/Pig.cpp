// Fill out your copyright notice in the Description page of Project Settings.


#include "Pig.h"

APig::APig()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APig::BeginPlay()
{
	Super::BeginPlay();
	PigFGuid = FGuid::NewGuid();
}

// void APig::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// }


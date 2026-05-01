// Fill out your copyright notice in the Description page of Project Settings.


#include "Decoration.h"

#include "NiagaraFunctionLibrary.h"
#include "Raccoon.h"

// Sets default values
ADecoration::ADecoration()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADecoration::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &ADecoration::OnHit);
}

// Called every frame
void ADecoration::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADecoration::OnHit(AActor* OverlappedActor, AActor* OtherActor)
{

	if (OtherActor->IsA(ARaccoon::StaticClass()))
	{
		SpawnVFX(DestroyVFX);
		Destroy();
	}
}

void ADecoration::SpawnVFX(UNiagaraSystem* VFX) const
{
	if (!VFX) return;
	USceneComponent* RootComp = GetRootComponent();
	if (!RootComp) return;

	// Spawn the Niagara component
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		VFX,
		GetActorLocation(),
		GetActorRotation(),
		FVector(1.0f),   // Scale
		true,            // Auto destroy
		true,            // Auto activate
		ENCPoolMethod::None,
		true    
	);
}
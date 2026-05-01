// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Decoration.h"
#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(lifeSpan);
	direction = GetActorForwardVector();
	OnActorBeginOverlap.AddDynamic(this, &AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation() + (direction * speed * DeltaTime);
	SetActorLocation(NewLocation);

	FRotator newRotation = GetActorRotation();
	newRotation.Yaw = newRotation.Yaw + DeltaTime * rotationSpeed;
	SetActorRotation(newRotation);
}

void AProjectile::OnHit(AActor* OverlappedActor, AActor* OtherActor)
{
	// Replace "AExcludedActor" with the class type you want to ignore
	if (OtherActor->IsA(ADecoration::StaticClass()))
	{
		SpawnVFX(DestroyVFX);
		SpawnVFX(DestroyVFX);
		SpawnVFX(DestroyVFX);
		Destroy();
	}
}

void AProjectile::SpawnVFX(UNiagaraSystem* VFX) const
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
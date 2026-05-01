// Fill out your copyright notice in the Description page of Project Settings.


#include "RoombaEnemy.h"

// Sets default values
ARoombaEnemy::ARoombaEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ARoombaEnemy::Attack()
{
	SpawnBP(projectile, FVector(0, 0, -25));
	Recoil();
	float timerLength = AttackDuration;

	GetWorld()->GetTimerManager().SetTimer(
		AttackTimer,
		[this]()
		{
			AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// AttackCollisionComponent->MarkRenderStateDirty();
			// AttackCollisionComponent->bHiddenInGame = true;
			FRotator rot = GetActorRotation();
			rot.Pitch = 0.0f;
			SetActorRotation(rot);
		},
		timerLength, 
		false
	);
}

void ARoombaEnemy::Recoil()
{
	//KnockbackStrength = 
	FVector KnockbackDirection = -GetActorForwardVector();
	KnockbackDirection.Y = 0;

	// Using Root Motion Dash
	TSharedPtr<FRootMotionSource_ConstantForce> KnockbackForce = MakeShareable(new FRootMotionSource_ConstantForce());
	KnockbackForce->InstanceName = FName("KnockbackForce");
	KnockbackForce->Priority = 5;
	KnockbackForce->AccumulateMode = ERootMotionAccumulateMode::Override;
	KnockbackForce->Force = KnockbackDirection * KnockbackStrength;
	KnockbackForce->Duration = KnockbackDuration;
	GetCharacterMovement()->ApplyRootMotionSource(KnockbackForce);
}

void ARoombaEnemy::SpawnBP(const TSubclassOf<AActor>& SpawnActor, const FVector& SpawnOffset) const
{
	if (!GetWorld() || !SpawnActor) return;
	FVector SpawnLocation = GetActorLocation() + SpawnOffset;
	FRotator SpawnRotation = GetActorRotation();
	SpawnRotation.Pitch = 0.0f;
	FActorSpawnParameters SpawnParameters;

	GetWorld()->SpawnActor<AActor>(SpawnActor, SpawnLocation, SpawnRotation, SpawnParameters);
}

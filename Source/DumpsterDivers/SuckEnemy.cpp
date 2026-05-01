// Fill out your copyright notice in the Description page of Project Settings.


#include "SuckEnemy.h"

#include "GameManagerInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Raccoon.h"

// Sets default values
ASuckEnemy::ASuckEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASuckEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (GetRootComponent())
	{
		telegraphScale = GetRootComponent()->GetComponentScale();
	}
	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();
}

// Called every frame
void ASuckEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isSuck)
	{
		Suck(DeltaTime);
	}
}

// Called to bind functionality to input
void ASuckEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASuckEnemy::Attack()
{
	isSuck = false;
	SpawnVFX(AttackVFX);
	AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
	// AttackCollisionComponent->MarkRenderStateDirty();
	// AttackCollisionComponent->bHiddenInGame = false;
	GetWorld()->GetTimerManager().SetTimer(
		AttackTimer,
		[this]()
		{
			AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// AttackCollisionComponent->MarkRenderStateDirty();
			// AttackCollisionComponent->bHiddenInGame = true;
		},
		AttackDuration, 
		false
	);
}

void ASuckEnemy::SpawnSuckVFX()
{
	if (!SuckVFX) return;

	USceneComponent* RootComp = GetRootComponent();
	if (!RootComp) return;

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		SuckVFX,
		RootComp,
		NAME_None,
		FVector::ZeroVector,
		AttackRotation,
		EAttachLocation::KeepRelativeOffset,
		false,              // AutoDestroy: false so we can stop it manually
		true,               // AutoActivate
		ENCPoolMethod::None
	);
	SuckVFXComp = NiagaraComp;
}

void ASuckEnemy::EndVFX() const
{
	SuckVFXComp->Deactivate();
}

void ASuckEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::Destroyed) OnEnemyDefeated.Broadcast(this);

	GetWorld()->GetTimerManager().ClearTimer(SuckTimer);
}

void ASuckEnemy::Start_Suck(float duration)
{
	isSuck = true;
	SpawnSuckVFX();
	GetWorld()->GetTimerManager().SetTimer(
		SuckTimer, 
		[this]() {EndVFX(); },
		3.0f, 
		false
	);
}

void ASuckEnemy::Start_telegraph_flash(float Duration)
{
	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();
	bStoredOriginalTransform = true;
	Super::Start_telegraph_flash(Duration);
	Start_Suck(Duration);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CHILD START"));
}

void ASuckEnemy::StartKnockback(const AActor* OtherActor, float StrengthScale)
{
	if (isSuck)
	{
		SetIsKnockback(true);
	}
	else
	{
		Super::StartKnockback(OtherActor, StrengthScale);
	}
}


void ASuckEnemy::Telegraph(float DeltaSeconds)
{
	FVector CurrentScale = GetActorScale();
	FVector TargetScale = telegraphScale * MaxInflate;
	FVector NewScale = FMath::VInterpTo(CurrentScale, TargetScale, DeltaSeconds, InflateSpeed);

	if (GetRootComponent())
	{
		GetRootComponent()->SetWorldScale3D(NewScale);
	}

	if (bStoredOriginalTransform)
	{
		// Shake strength
		float PosShakeStrength = 2.0f * NewScale.X * NewScale.X; // Units
		float RotShakeStrength = 3.0f * NewScale.X * NewScale.X; // Degrees
		// if (NewScale.X > 1.1)
		// {
		// 	SpawnVFX(SmokeVFX);
		// }
		// Position shake
		FVector ShakeOffset = FVector(
			FMath::FRandRange(-PosShakeStrength, PosShakeStrength),
			FMath::FRandRange(-PosShakeStrength, PosShakeStrength),
			0); // Only shake in XY

		// Rotation shake
		FRotator ShakeRotation = FRotator(
			FMath::FRandRange(-RotShakeStrength, RotShakeStrength),
			FMath::FRandRange(-RotShakeStrength, RotShakeStrength),
			FMath::FRandRange(-RotShakeStrength, RotShakeStrength));

		// Apply shake
		SetActorLocation(OriginalLocation + ShakeOffset);
		SetActorRotation(OriginalRotation + ShakeRotation);
	}

}

void ASuckEnemy::endTelegraph()
{
	SetActorLocation(OriginalLocation);
	SetActorRotation(OriginalRotation);
	bStoredOriginalTransform = false;
	if (GetRootComponent())
	{
		GetRootComponent()->SetWorldScale3D(telegraphScale);
	}
}


void ASuckEnemy::Suck(float DeltaTime)
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	if (!gameInstance) return;
	ARaccoon* player = Cast<ARaccoon>(gameInstance->GetWorld()->GetFirstPlayerController()->GetPawn());

	FVector EnemyLocation = GetActorLocation();
	FVector PlayerLocation = player->GetActorLocation();

	float Distance = FVector::Dist(EnemyLocation, PlayerLocation);
	
	if (Distance > SuctionRadius) return;

	FVector Direction = (EnemyLocation - PlayerLocation).GetSafeNormal();
	float ForceMagnitude = 1000 * SuctionStrength * (1.0f - Distance / SuctionRadius) * GetActorScale().X * GetActorScale().X; // Linear falloff
	FVector PullForce = Direction * ForceMagnitude * DeltaTime;

	if (UCharacterMovementComponent* Movement = player->GetCharacterMovement())
	{
		Movement->Velocity += PullForce;
		Movement->UpdateComponentVelocity(); // Force update
		
		FVector Velocity = Movement->Velocity;
		// FString VelocityString = FString::Printf(TEXT("Velocity: X=%.2f Y=%.2f Z=%.2f"), Velocity.X, Velocity.Y, Velocity.Z);
		// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, VelocityString);
	}
}

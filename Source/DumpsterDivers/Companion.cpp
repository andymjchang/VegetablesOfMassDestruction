// Fill out your copyright notice in the Description page of Project Settings.


#include "Companion.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ACompanion::ACompanion()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACompanion::BeginPlay()
{
	Super::BeginPlay();
	
	// Set acceptance radius value in blackboard
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
		{
			// Set your blackboard values
			BlackboardComp->SetValueAsFloat("AcceptRadius", FMath::FRandRange(300.f, 600.f));
		}
	}

	SpawnPoofEffect();
}

// Called every frame
// void ACompanion::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// }

// Called to bind functionality to input
void ACompanion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// void ACompanion::HopForward()
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("hot foward")));
//
// 	Jump();
// 	//SetActorLocation(GetActorLocation() + GetActorForwardVector() * 100.0f);
// 	
// }

void ACompanion::SpawnPoofEffect() const
{
	SpawnVFX(PoofVFX, GetActorLocation(), PoofEffectScale);
}

void ACompanion::SpawnVFX(UNiagaraSystem* Effect, const FVector& SpawnLocation, const FVector& EffectScale) const
{
	if (!Effect) return;

	// Spawn the Niagara system at the specified world location
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),        // World context
		Effect,            // The Niagara system to spawn
		SpawnLocation,     // World location for the effect
		FRotator::ZeroRotator, // Rotation
		EffectScale,     // Scale 
		true,              // Auto destroy
		true,              // Auto activate
		ENCPoolMethod::AutoRelease // pooling
	);
}




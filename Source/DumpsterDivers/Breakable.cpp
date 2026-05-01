// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable.h"

// Sets default values
ABreakable::ABreakable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set default item spawn parameters
	LaunchAngleDegrees = 80.f;
	LaunchMagnitude = 350.f;
	LaunchTorque = 800.f;
	DelayUntilInteractableSeconds = 0.5f;
	CanRespawn = true;
	DelayUntilRespawnSeconds = 5.0f;

	// Set up hitbox
	HurtBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Hurt Box"));
	HurtBox->SetGenerateOverlapEvents(true);
	RootComponent = HurtBox;

	// Set up mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
}

// Called when the game starts or when spawned
void ABreakable::BeginPlay()
{
	Super::BeginPlay();

	HurtBox->OnComponentBeginOverlap.AddDynamic(this, &ABreakable::OnHit);	
}

void ABreakable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReactivateTimer);
	GetWorld()->GetTimerManager().ClearTimer(ItemTimer);
}

// Called every frame
void ABreakable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* otherComponent, 
		   			   int32 OtherIndex, bool bFromSweep, const FHitResult &Hit)
{
	if (OtherActor->IsA(ARaccoon::StaticClass())) {

		// Spawn item drop
		FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);
		FRotator SpawnRotation(0.f, 0.f, 0.f); 
		AItem* SpawnedItem = GetWorld()->SpawnActorDeferred<AItem>(ItemClass, FTransform(SpawnRotation, SpawnLocation));

		if (!SpawnedItem) return;

		FVector DirectionToBreakable = (SpawnLocation - OtherActor->GetActorLocation()).GetSafeNormal() * FVector(1.f, 1.f, 0.f);
		FVector RightVector = FVector::CrossProduct(DirectionToBreakable, FVector::UpVector).GetSafeNormal();
		FQuat QuatRotation = FQuat(RightVector, FMath::DegreesToRadians(LaunchAngleDegrees));
		FVector LaunchDirection = QuatRotation.RotateVector(DirectionToBreakable);
		FVector LaunchVelocity = LaunchDirection * LaunchMagnitude;

		// Enable collision after delay
		USphereComponent* ItemCollision = SpawnedItem->GetSphereComponent();
		UStaticMeshComponent* ItemMesh = SpawnedItem->GetMeshComponent();
		
		SpawnedItem->SetItem(ItemDataTableRow);
		if (ItemCollision) ItemCollision->SetGenerateOverlapEvents(false);
		if (ItemMesh) ItemMesh->SetCollisionEnabled(ECollisionEnabled::ProbeOnly);
		GetWorld()->GetTimerManager().SetTimer(
			ItemTimer, 
			[ItemCollision, ItemMesh, RightVector, this]()
			{
				if (!this) return;
				if (ItemCollision) ItemCollision->SetGenerateOverlapEvents(true);
				if (ItemMesh) {
					float DeltaTime = GetWorld()->GetDeltaSeconds();
					ItemMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
					FVector SpinTorque = RightVector * -LaunchTorque; 
					ItemMesh->AddTorqueInRadians(SpinTorque, FName(TEXT("None")), true);
				}
				if (this) this->Destroy();
			},
			DelayUntilInteractableSeconds, 
			false 
		);

		SpawnedItem->FinishSpawning(FTransform(SpawnRotation, SpawnLocation));
	
		// Apply upwards force
		if (ItemMesh) ItemMesh->AddImpulse(LaunchVelocity, FName(TEXT("None")), true);

		// Apply torque
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		FVector SpinTorque = RightVector * -LaunchTorque; 
		if (ItemMesh) ItemMesh->AddTorqueInRadians(SpinTorque, FName(TEXT("None")), true);

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		
		if (!CanRespawn) return;

		// Deactivate actor until timer up
		GetWorld()->GetTimerManager().SetTimer(
			ReactivateTimer, 
			[this]()
			{
				SetActorHiddenInGame(false);
				SetActorEnableCollision(true);
			},
			DelayUntilRespawnSeconds, 
			false 
		);
	}
	
}


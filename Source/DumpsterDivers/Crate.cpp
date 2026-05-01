// Fill out your copyright notice in the Description page of Project Settings.


#include "Crate.h"

#include "Item.h"
#include "Raccoon.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

// Sets default values
ACrate::ACrate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Durability = 1;
	ClearCrateDebrisDelay = 5.f;
	LaunchAngleDegrees = 80.f;
	LaunchMagnitude = 350.f;
	LaunchTorque = 1000.f;
	DelayUntilInteractableSeconds = 0.5f;
	IsActive = true;

	CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Crate"));
	SetRootComponent(CrateMesh);
	CrateMesh->SetSimulatePhysics(true);
	CrateMesh->SetCollisionProfileName(FName("BlockALl"));
	CrateMesh->BodyInstance.bOverrideMass = true;
	CrateMesh->BodyInstance.SetMassOverride(200.f);
	
	HurtBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HurtBox"));
	HurtBox->SetupAttachment(CrateMesh);
	HurtBox->SetGenerateOverlapEvents(true);
}

void ACrate::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* otherComponent, int32 OtherIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (Durability == -1)
	{
		return; //unbreakable
	}
	if (OtherActor->IsA(ARaccoon::StaticClass()))
	{
		--Durability;
		if (Durability <= 0 && IsActive) BreakCrate(OtherActor);	
	}
}

void ACrate::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	TArray<AActor*> OverlappedActors;
	GetOverlappingActors(OverlappedActors, APawn::StaticClass());
	if (OverlappedActors.IsEmpty())
	{
		CrateMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		HurtBox->SetGenerateOverlapEvents(true);
	}
}

void ACrate::SetItemData(FDataTableRowHandle NewItemData)
{
	ItemData = NewItemData;
}

void ACrate::BreakCrate(AActor* OtherActor)
{
	// Spawn item drop
	FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);
	FRotator SpawnRotation(0.f, 0.f, 0.f); 
	AItem* SpawnedItem = GetWorld()->SpawnActorDeferred<AItem>(ItemClass, FTransform(SpawnRotation, SpawnLocation));

	if (!SpawnedItem) return;

	float RandomAngle = FMath::RandRange(0.f, 360.f);
	FVector XYDirection = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)), FMath::Sin(FMath::DegreesToRadians(RandomAngle)), 0.f).GetSafeNormal();
	FVector RightVector = FVector::CrossProduct(XYDirection, FVector::UpVector).GetSafeNormal();
	FQuat QuatRotation = FQuat(RightVector, FMath::DegreesToRadians(LaunchAngleDegrees));
	FVector LaunchDirection = QuatRotation.RotateVector(XYDirection);
	FVector LaunchVelocity = LaunchDirection * LaunchMagnitude;

	// Enable collision after delay
	USphereComponent* ItemCollision = SpawnedItem->GetSphereComponent();
	UStaticMeshComponent* ItemMesh = SpawnedItem->GetMeshComponent();
	
	SpawnedItem->SetItem(ItemData);
	if (ItemCollision) ItemCollision->SetGenerateOverlapEvents(false);
	if (ItemMesh) ItemMesh->SetCollisionEnabled(ECollisionEnabled::ProbeOnly);
	GetWorld()->GetTimerManager().SetTimer(
		ItemTimer, 
		[ItemCollision, ItemMesh, RightVector, this]()
		{
			if (!this) return;
			if (ItemCollision) ItemCollision->SetGenerateOverlapEvents(true);
			if (ItemMesh) {
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
				FVector SpinTorque = RightVector * -LaunchTorque; 
				ItemMesh->AddTorqueInRadians(SpinTorque, FName(TEXT("None")), true);
			}
			
			// Destroy();
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

	// Disable crate after break
	IsActive = false;
	SetActorHiddenInGame(true);
	CrateMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CrateMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	// Spawn geometry collection
	CrateMesh->SetRelativeScale3D(CrateMesh->GetRelativeScale3D() * 0.4f);
	
	AGeometryCollectionActor* FractureActor = GeometryCollection ?
		GetWorld()->SpawnActor<AGeometryCollectionActor>(
			AGeometryCollectionActor::StaticClass(),
			CrateMesh->GetComponentLocation(),
			CrateMesh->GetComponentRotation()) :
		nullptr;

	UGeometryCollectionComponent* FractureComponent = FractureActor ?
		FractureActor->GetGeometryCollectionComponent() :
		nullptr;
	
	if (FractureComponent)
	{
		FractureComponent->SetRestCollection(GeometryCollection);
		FractureComponent->SetSimulatePhysics(true);
	}

	GetWorld()->GetTimerManager().SetTimer(
		CleanupTimer,
		[this, FractureComponent]()->void
		{
			if (FractureComponent) FractureComponent->DestroyComponent();
			if (this) Destroy();
		},
		ClearCrateDebrisDelay,
		false
	);
}

// Called when the game starts or when spawned
void ACrate::BeginPlay()
{
	Super::BeginPlay();

	HurtBox->OnComponentBeginOverlap.AddDynamic(this, &ACrate::OnHit);
	CrateMesh->OnComponentEndOverlap.AddDynamic(this, &ACrate::OnEndOverlap);
}

void ACrate::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ItemTimer);
	GetWorld()->GetTimerManager().ClearTimer(CleanupTimer);
}

// Called every frame
void ACrate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "LevelManagerSubsystem.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "DumpsterDivers/CrateComponent.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize variables
	SectionID = 1;

	// Initialize components
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);
	Capsule->ShapeColor = FColor::Cyan;
	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(44.f);
	
	ForwardIndicator = CreateDefaultSubobject<UArrowComponent>(TEXT("ForwardIndicator"));
	ForwardIndicator->SetupAttachment(RootComponent);
	ForwardIndicator->SetArrowColor(FColor::Cyan);
}

AEnemy* AEnemySpawner::SpawnEnemy(FEnemySpawnConfig EnemySpawn)
{
	FVector SpawnHeightOffset = FVector(0.f, 0.f, 2400.f);
	FTransform SpawnTransform = GetActorTransform();
	AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(
		EnemySpawn.EnemyClass,
		SpawnTransform.GetLocation() + SpawnHeightOffset,
		SpawnTransform.Rotator()
	);

	if (!SpawnedEnemy) return nullptr;

	UCrateComponent* CrateComponent = SpawnedEnemy->GetComponentByClass<UCrateComponent>();
	if (CrateComponent && !EnemySpawn.ItemData.IsNull()) CrateComponent->SetCrateItemData(EnemySpawn.ItemData);
	
	ULevelManagerSubsystem* LevelManager = GetWorld()->GetSubsystem<ULevelManagerSubsystem>();
	if (LevelManager)
	{
		SpawnedEnemy->OnEnemyDefeated.AddDynamic(LevelManager, &ULevelManagerSubsystem::EnemyDefeatedEvent);
		SpawnedEnemy->SectionID = this->SectionID;
		SpawnedEnemy->bHasLanded = false;
	}
	
	return SpawnedEnemy;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


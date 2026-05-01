// Fill out your copyright notice in the Description page of Project Settings.


#include "SectionDoor.h"
#include "LevelManagerSubsystem.h"
#include "Algo/RandomShuffle.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "DumpsterDivers/Raccoon.h"

// Sets default values
ASectionDoor::ASectionDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize actor properties
	SectionID = 1;
	MaxActiveEnemies = 5;
	SpawnDelay = 3.f;
	RandomizeNextEnemySpawn = false;
	EnemySpawnList = TArray<FEnemySpawnConfig>();
	RotationDuration = 1.f;
	CurveSteepness = 5.0f;
	CurveMidpoint = 0.9f;  
	RotationAngleDegrees = FRotator(0.f, -145.f, 0.f);
	RotationTimeElapsed = 0.0f;
	InitialRotation = FRotator(0, 0, 0);
	IsMoving = false;
	IsOpened = false;
	EventQueued = false;
	
	// Initialize components
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->ShapeColor = FColor::Green;
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->bDynamicObstacle = true;
	TriggerBox->SetRelativeLocation(FVector(50.f, -45.f, 70.f));
	TriggerBox->SetBoxExtent(FVector(80.f, 10.f, 75.f));

	BlockingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox"));
	BlockingBox->SetupAttachment(RootComponent);
	BlockingBox->bDynamicObstacle = true;
	BlockingBox->SetRelativeLocation(FVector(50.f, -85.f, 70.f));
	BlockingBox->SetBoxExtent(FVector(80.f, 100.f, 75.f));

	RotationPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RotationPoint"));
	RotationPoint->SetupAttachment(RootComponent);
	OriginalRotation = RotationPoint->GetRelativeRotation();

	DoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMeshComponent->SetupAttachment(RotationPoint);
	DoorMeshComponent->SetRelativeLocation(FVector(50.f, 0.f, 25.f));

	DoorRotationAxis = CreateDefaultSubobject<UArrowComponent>(TEXT("DoorRotationAxis"));
	DoorRotationAxis->SetupAttachment(RootComponent);
	DoorRotationAxis->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DoorRotationAxis->SetArrowLength(500.f);
	DoorRotationAxis->SetArrowSize(0.5f);
}

// Called when the game starts or when spawned
void ASectionDoor::BeginPlay()
{
	Super::BeginPlay();

	OnSectionStart.AddDynamic(this, &ASectionDoor::SectionStartEvent);
	OnSectionEnd.AddDynamic(this, &ASectionDoor::SectionEndEvent);

	if (SectionID == 1) OnSectionStart.Broadcast();

	if (DoorMeshComponent) DoorMeshComponent->SetCollisionProfileName(FName("BlockAll"));

	// Register door in level manager
	ULevelManagerSubsystem* LevelManager = GetWorld()->GetSubsystem<ULevelManagerSubsystem>();
	if (LevelManager) LevelManager->RegisterDoor(this);

	// Insert enemies into queue
	if (RandomizeNextEnemySpawn) Algo::RandomShuffle(EnemySpawnList);
	for (FEnemySpawnConfig EnemySpawn : EnemySpawnList) EnemyQueue.Enqueue(EnemySpawn);

	// Connect trigger box
	if (TriggerBox)	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASectionDoor::OnEnterDoor);	
}

void ASectionDoor::OnEnterDoor(UPrimitiveComponent* HitComponent, AActor* OtherActor,
							   UPrimitiveComponent* otherComponent, int32 OtherIndex,
							   bool bFromSweep, const FHitResult& Hit)
{
	if (OtherActor->IsA(ARaccoon::StaticClass()))
	{
		CloseDoor();
		ULevelManagerSubsystem* LevelManager = GetWorld()->GetSubsystem<ULevelManagerSubsystem>();
		if (LevelManager) LevelManager->StartSection(SectionID + 1);
	}
}

// Called every frame
void ASectionDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsMoving)
	{
		RotationTimeElapsed += DeltaTime;
		float NormalizedTime = RotationTimeElapsed / RotationDuration;

		float Alpha = 1.0f / (1.0f + FMath::Exp(-CurveSteepness * (NormalizedTime - CurveMidpoint)));

		Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
		
		// Interpolate rotation
		FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
		RotationPoint->SetRelativeRotation(NewRotation);
		
		// Check if finished
		if (Alpha >= 1.0f)
		{
			IsMoving = false;
		}
	}
}

void ASectionDoor::OpenDoor()
{
	if (IsOpened) return;
	
	InitialRotation = RotationPoint->GetRelativeRotation();
	TargetRotation = OriginalRotation + RotationAngleDegrees;
	
	RotationTimeElapsed = 0.f;
	BlockingBox->SetCollisionProfileName(FName("OverlapAllDynamic"));

	IsMoving = true;
	IsOpened = true;

	OnSectionEnd.Broadcast();
}

void ASectionDoor::CloseDoor()
{
	if (!IsOpened) return;

	InitialRotation = RotationPoint->GetRelativeRotation();
	TargetRotation = OriginalRotation;

	RotationTimeElapsed = 0.f;
	BlockingBox->SetRelativeLocation(BlockingBox->GetRelativeLocation() * FVector(1.f, -1.f, 1.f));
	BlockingBox->SetCollisionProfileName(FName("BlockAll"));
	
	IsMoving = true;
	IsOpened = false;

	OnSectionStart.Broadcast();
}


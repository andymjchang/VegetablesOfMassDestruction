// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManagerSubsystem.h"
#include "DumpsterDivers/EnemyController.h"
#include "DumpsterDivers/EnemyIndicatorUI.h"
#include "DumpsterDivers/GUISubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void ULevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Doors.Empty();
	// EnemyCounts.Empty();
	Enemies.Empty();
	Spawners.Empty();
	
	UE_LOG(LogTemp, Log, TEXT("Level manager subsystem initialized"));
}

void ULevelManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	if (GetWorld()->GetTimerManager().IsTimerActive(SpawnTimer))
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
}

void ULevelManagerSubsystem::RegisterDoor(ASectionDoor* NewDoor)
{
	if (!NewDoor) return;
	
	Doors.Add(NewDoor);
	Doors.Sort([](ASectionDoor& A, ASectionDoor& B) -> bool
	{
		return A.SectionID < B.SectionID;
	});
}

void ULevelManagerSubsystem::CollectSpawnersBySection()
{
	Spawners.SetNum(Doors.Num());

	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundSpawners);
	for (AActor* Actor : FoundSpawners)
	{
		AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor);
		if (!Spawner) continue;

		int SectionID = Spawner->SectionID - 1;
		if (SectionID < 0 || SectionID >= Doors.Num()) continue;

		Spawners[SectionID].Add(Spawner);
	}
}

void ULevelManagerSubsystem::CountPrePlacedEnemies()
{
	Enemies.SetNum(Doors.Num() + 1);
	
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), FoundEnemies);
	
	for (AActor* Actor : FoundEnemies)
	{
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (!Enemy) continue;
		
		int SectionIndex = Enemy->SectionID - 1;
		if (SectionIndex < 0 || SectionIndex >= Doors.Num() + 1) continue;

		Enemy->OnEnemyDefeated.AddDynamic(this, &ULevelManagerSubsystem::EnemyDefeatedEvent);

		// Initially disable enemy AI
		AEnemyController* Controller = Cast<AEnemyController>(Enemy->GetController());
		UBrainComponent* BrainComponent = (Controller) ? Controller->GetBrainComponent() : nullptr;
		if (BrainComponent) BrainComponent->StopLogic(FString("Unactivated"));

		// Reactivate on entering section
		OnSectionEntered.AddDynamic(Enemy, &AEnemy::ResumeAIMovement);
		
		// ++EnemyCounts[SectionIndex];
		Enemies[SectionIndex].Add(Enemy);
	}
}

void ULevelManagerSubsystem::InitLevelManager()
{
	CountPrePlacedEnemies();
	CollectSpawnersBySection();
	FetchEnemyIndicatorUI();
	StartSection(1);
}

void ULevelManagerSubsystem::StartSection(int SectionNumber)
{
	// Handle case for enemies after last door
	OnSectionEntered.Broadcast(SectionNumber);

	// Set up enemy indicators
	if (EnemyIndicatorUI) EnemyIndicatorUI->InitializeSectionIndicators(SectionNumber);
	
	// Begin next section
	int SectionIndex = SectionNumber - 1;
	if (SectionIndex < 0 || SectionIndex >= Doors.Num()) return;
	SpawnUntilFull(SectionIndex);
	CheckForSectionComplete(SectionIndex);
}

AEnemySpawner* ULevelManagerSubsystem::SelectRandomSpawner(int SectionIndex)
{
	if (SectionIndex < 0 || SectionIndex >= Doors.Num()) return nullptr;
	
	TArray<AEnemySpawner*>& SectionSpawners = Spawners[SectionIndex];
	if (!SectionSpawners.IsEmpty())
	{
		int RandomIndex = UKismetMathLibrary::RandomIntegerInRange(0, SectionSpawners.Num() - 1);
		return SectionSpawners[RandomIndex];
	}

	return nullptr;
}

void ULevelManagerSubsystem::CheckForSectionComplete(int SectionIndex)
{
	// int& RemainingEnemies = EnemyCounts[SectionIndex];
	int RemainingEnemies = Enemies[SectionIndex].Num();
	ASectionDoor* Door = Doors[SectionIndex];
	
	if (RemainingEnemies <= 0 && (Door->EnemyQueue.IsEmpty() || Spawners[SectionIndex].IsEmpty()))
		Door->OpenDoor();
}

void ULevelManagerSubsystem::SpawnUntilFull(int SectionIndex)
{
	if (SectionIndex < 0 || SectionIndex >= Doors.Num()) return;
	
	// int& RemainingEnemies = EnemyCounts[SectionIndex];
	int RemainingEnemies = Enemies[SectionIndex].Num();
	ASectionDoor* Door = Doors[SectionIndex];
	TQueue<FEnemySpawnConfig>* EnemyQueue = &Door->EnemyQueue;
	int SpawnDelay = Door->SpawnDelay;
	int MaxActiveEnemies = Door->MaxActiveEnemies;
	
	if (GetWorld()->GetTimerManager().IsTimerActive(SpawnTimer) ||
		EnemyQueue->IsEmpty() ||
		RemainingEnemies >= MaxActiveEnemies)
		return;

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		[this, EnemyQueue, SectionIndex, MaxActiveEnemies]()->void
		{
			if (!EnemyQueue || EnemyQueue->IsEmpty() || Enemies[SectionIndex].Num() >= MaxActiveEnemies)
			{
				GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
				return;
			}
			
			// Pop enemy queue
			FEnemySpawnConfig NextEnemy;
			EnemyQueue->Dequeue(NextEnemy);
				
			// Choose a random spawn for this section
			AEnemySpawner* Spawner = SelectRandomSpawner(SectionIndex);
			if (!Spawner) return;
			
			// Spawn enemy
			AEnemy* Enemy = Spawner->SpawnEnemy(NextEnemy);
			Enemies[SectionIndex].Add(Enemy);
		},
		SpawnDelay,
		true);
}

void ULevelManagerSubsystem::EnemyDefeatedEvent(AEnemy* Enemy)
{
	// Change section number to 0-based indexing
	int SectionIndex = Enemy->SectionID - 1;
	if (SectionIndex < 0 || SectionIndex >= Doors.Num()) return;

	// --EnemyCounts[SectionIndex];
	Enemies[SectionIndex].Remove(Enemy);
	
	// Start Spawn Loop
	SpawnUntilFull(SectionIndex);
	CheckForSectionComplete(SectionIndex);
}

TArray<AEnemy*>* ULevelManagerSubsystem::GetEnemiesInSection(int SectionID)
{
	return &Enemies[SectionID - 1];
}

void ULevelManagerSubsystem::FetchEnemyIndicatorUI()
{
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UGUISubsystem* GUISubsystem = GameInstance ? GameInstance->GetSubsystem<UGUISubsystem>() : nullptr;
	EnemyIndicatorUI = GUISubsystem ? GUISubsystem->EnemyIndicatorUI : nullptr;
}





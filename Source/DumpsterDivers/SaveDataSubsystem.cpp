// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveDataSubsystem.h"

#include "GUISubsystem.h"
#include "Raccoon.h"
#include "Kismet/GameplayStatics.h"

void USaveDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Set player data to default values
	PlayerData = UPlayerDefaults::DefaultPlayerData;
}

void USaveDataSubsystem::SaveData()
{
	// Save player data
	ARaccoon* Player = Cast<ARaccoon>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(Player)) return;
	
	PlayerData.MaxHealth = Player->MaxHealth;
	PlayerData.CurrentHealth = Player->CurrentHealth;
	PlayerData.Power = Player->power;
	PlayerData.CritChance = Player->crit_chance;
	PlayerData.CritMultiplier = Player->crit_multiplier;
	PlayerData.Speed = Player->speed;
	PlayerData.AttackSpeed = Player->attack_speed;
	PlayerData.EggBaseDamage = Player->EggBaseDamage;
	PlayerData.EggBaseRadius = Player->EggBaseRadius;
	PlayerData.DashCooldown = Player->DashCooldown;
	PlayerData.MaxDashCount = Player->MaxDashCount;

	// Save inventory data
	UGameManagerInstance* GameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
	UInventorySubsystem* InventorySubsystem = (GameInstance) ? GameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;
	if (!IsValid(InventorySubsystem)) return;
	PlayerData.Inventory = InventorySubsystem->weaponsList;

	// Save companions data
	TArray<AActor*> CompanionActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACompanion::StaticClass(), CompanionActors);
	for (AActor* CompanionActor : CompanionActors)
	{
		CompanionsData.Add(CompanionActor->GetClass());
	}
}

void USaveDataSubsystem::LoadData()
{
	// Load player data
	ARaccoon* Player = Cast<ARaccoon>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!IsValid(Player)) return;
	
	Player->MaxHealth = PlayerData.MaxHealth;
	Player->CurrentHealth = PlayerData.CurrentHealth;
	Player->power = PlayerData.Power;
	Player->crit_chance = PlayerData.CritChance;
	Player->crit_multiplier = PlayerData.CritMultiplier;
	Player->speed = PlayerData.Speed;
	Player->attack_speed = PlayerData.AttackSpeed;
	Player->EggBaseDamage = PlayerData.EggBaseDamage;
	Player->EggBaseRadius = PlayerData.EggBaseRadius;
	Player->DashCooldown = PlayerData.DashCooldown;
	Player->MaxDashCount = PlayerData.MaxDashCount;

	// Load inventory data
	UInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UInventorySubsystem>();
	if (!IsValid(InventorySubsystem) || PlayerData.Inventory.IsEmpty()) return;
	InventorySubsystem->SetInventory(PlayerData.Inventory);
	InventorySubsystem->RefreshInventoryWeaponEffects();

	// Load companions data
	for (TSubclassOf<AActor> CompanionClass : CompanionsData)
	{
		if (IsValid(CompanionClass)) Player->SpawnCompanion(CompanionClass);
	}
}

void USaveDataSubsystem::ClearData()
{
	PlayerData = UPlayerDefaults::DefaultPlayerData;
	
	// UInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UInventorySubsystem>();
	// if (!IsValid(InventorySubsystem)) return;
	//
	// PlayerData.Inventory = TArray<FWeaponStruct>;

	CompanionsData.Empty();
}

void USaveDataSubsystem::ModifyHealth(float Health)
{
	PlayerData.CurrentHealth = FMath::Clamp(Health, 0, PlayerData.MaxHealth);
}

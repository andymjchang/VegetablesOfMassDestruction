// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputSubsystems.h"
#include "Tickable.h"
#include "GameManagerInstance.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Tag.h"
#include "InventorySubsystem.generated.h"

class UGUISubsystem;
class ARaccoon;

USTRUCT(BlueprintType)
struct DUMPSTERDIVERS_API FWeaponStruct: public FTableRowBase
{
	GENERATED_BODY()

	// Static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	UStaticMesh* heldMesh;

	// Static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	UStaticMesh* attackMesh;

	// Static mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	UStaticMesh* backMesh;

	// List of hitboxes (using Box components as an example)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	TArray<UBoxComponent*> hitboxes;

	//ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	FName id;

	// Icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	UTexture2D* icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Struct")
	UAnimMontage* AttackMontage;

	// combolength
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	int comboLength;
	
	//swing time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	TArray<float> attackLength;
	
	// recovery time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	TArray<float> recoveryLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	TArray<float> damageAmount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	TArray<FTag> tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Struct")
	float knockbackMult;
	
	// Constructor
	FWeaponStruct()
		: heldMesh(nullptr)
		, attackMesh(nullptr)
		, backMesh(nullptr)
		, id(FName(TEXT("Empty")))
		, icon(nullptr)
		, AttackMontage(nullptr)
		, comboLength(1)
		, attackLength({1.0f})
		, recoveryLength({1.0f})
		, damageAmount({1.0f})
		, tags({})
		, knockbackMult(1.0f)
	{
	}

	// copy constructor 
	FWeaponStruct(const FWeaponStruct& other)
		: heldMesh(other.heldMesh)
		, attackMesh(other.attackMesh)
		, backMesh(other.backMesh)
		, id(other.id)
		, icon(other.icon)
		, AttackMontage(other.AttackMontage)
		, comboLength(other.comboLength)
		, attackLength(other.attackLength)
		, recoveryLength(other.recoveryLength)
		, damageAmount(other.damageAmount)
		, tags(other.tags)
		, knockbackMult(other.knockbackMult)
	{
	}
};

UCLASS()
class DUMPSTERDIVERS_API UInventorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UInventorySubsystem();
	
	TArray<FWeaponStruct> weaponsList;
	
	int currentWeaponIndex;
	
	UGameManagerInstance* gameInstance; 

	UDataTable* weaponTable;
	
	UDataTable* tagVFXTable;

	UDataTable* TagTable;

protected:

public:	
	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void InventoryScrollEvent(const FInputActionValue& Value);
	void UpdateCurrentGUI();

	UFUNCTION(BlueprintCallable)
	int GetCurrentWeaponIndex();

	UFUNCTION(BlueprintCallable)
	void SetCurrentWeaponIndex(int value);
	
	UFUNCTION(BlueprintCallable)
	FName GetItemIDAt(int Index);

	UFUNCTION(BlueprintCallable)
	void InitInventory(UDataTable* weaponsTable, UDataTable* tagsVFXTable);

	UFUNCTION(BlueprintCallable)
	void ClearInventory();

	UFUNCTION(BlueprintCallable)
	void SetInventory(TArray<FWeaponStruct> NewInventory);
	
	//UFUNCTION(BlueprintCallable)
	const FWeaponStruct* GetCurrentWeapon();
	
	void UpdatePlayerWeaponMesh(FName id, FName id2);
	void RemoveItemFromInventory(const int Index);
	int GetNextWeaponIndex() const;
	void SetMeshForAttack(bool bIsEnding);
	bool AddNewItem(FName id, int comboLength);
	void AssignWeaponEffect(int index, FName id);
	void RefreshInventoryWeaponEffects();
};



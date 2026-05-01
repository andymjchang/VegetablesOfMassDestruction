// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySubsystem.h"
#include "GUISubsystem.h"
#include "Raccoon.h"
#include "DT_Tag.h"
#include "Tag.h"

void UInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
	// UDT_Tag* TagDataTableObject = NewObject<UDT_Tag>();
	// if (TagDataTableObject) return;
	// TagTable = TagDataTableObject->CreateDT_Tag();
	// TagTable->AddToRoot();
	UE_LOG(LogTemp, Log, TEXT("Inventory Manager Subsystem Initialized"));
}

// Sets default values
UInventorySubsystem::UInventorySubsystem()
{
	gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
}

void UInventorySubsystem::InitInventory(UDataTable* weaponsTable, UDataTable* tagsVFXTable)
{
	if (!weaponsTable) return;
	if (!tagsVFXTable) return;

	tagVFXTable = tagsVFXTable; 
	weaponTable = weaponsTable;

	UDT_Tag* TagDataTableObject = NewObject<UDT_Tag>();
	if (!TagDataTableObject) return;
	TagTable = TagDataTableObject->CreateDT_Tag();
	TagTable->AddToRoot();
	
	// grav the vfx
	// and shove it into tags
	TagDataTableObject->UpdatewithVFX(TagTable, tagsVFXTable); 

	currentWeaponIndex = 0; 
	weaponsList.SetNum(2);
	
	ClearInventory();
}

void UInventorySubsystem::ClearInventory()
{
	if (!IsValid(weaponTable)) return;
	for (int i = 0; i < weaponsList.Num(); i++)
	{
		weaponsList[i] = *weaponTable->FindRow<FWeaponStruct>(FName("Empty"), "");
	}
	UpdatePlayerWeaponMesh(FName("Empty"), FName("Empty"));
}

void UInventorySubsystem::SetInventory(TArray<FWeaponStruct> NewInventory)
{
	if (!weaponTable) return;
	for (int i = 0; i < weaponsList.Num(); ++i)
	{
		weaponsList[i] = *weaponTable->FindRow<FWeaponStruct>(NewInventory[i].id, "");
	}
	UpdatePlayerWeaponMesh(GetItemIDAt(currentWeaponIndex), GetItemIDAt(GetNextWeaponIndex()));
}

void UInventorySubsystem::InventoryScrollEvent(const FInputActionValue& Value) 
{
    // Set new current weapon index
	float scrollDirection = Value.Get<float>();
	if (scrollDirection > 0) {
		currentWeaponIndex = (currentWeaponIndex + 1 >= weaponsList.Num()) ? 0 : currentWeaponIndex + 1;
	} else if (scrollDirection < 0) {
		currentWeaponIndex = (currentWeaponIndex - 1 < 0) ? weaponsList.Num() - 1 : currentWeaponIndex - 1;
	}
	UpdateCurrentGUI();
}

void UInventorySubsystem::UpdateCurrentGUI()
{
	if (!gameInstance) return;
	const UGUISubsystem* GUI = gameInstance->GetSubsystem<UGUISubsystem>(); 
	if (!GUI) return; 
	GUI->RaccoonGUI->UpdateHighlightInventory(currentWeaponIndex, weaponsList.Num()); 
	UpdatePlayerWeaponMesh(weaponsList[currentWeaponIndex].id, GetItemIDAt(GetNextWeaponIndex()));
}

const FWeaponStruct* UInventorySubsystem::GetCurrentWeapon()
{
	return &weaponsList[currentWeaponIndex];
}

int UInventorySubsystem::GetCurrentWeaponIndex()
{
	return currentWeaponIndex;
}

void UInventorySubsystem::SetCurrentWeaponIndex(int value)
{
	currentWeaponIndex = value;
    
	// show it in gui 
	if (!gameInstance) return; 
	UGUISubsystem* gui = gameInstance->GetSubsystem<UGUISubsystem>(); 
	if (!gui) return; 
	gui->RaccoonGUI->UpdateHighlightInventory(currentWeaponIndex, weaponsList.Num()); 
    
	UpdatePlayerWeaponMesh(weaponsList[currentWeaponIndex].id, GetItemIDAt(GetNextWeaponIndex()));
}

FName UInventorySubsystem::GetItemIDAt(int Index)
{
	return (weaponsList.IsEmpty() || Index < 0 || Index >= weaponsList.Num()) ? FName("Empty") : weaponsList[Index].id;
}

bool UInventorySubsystem::AddNewItem(FName id, int comboLength) {
	// TODO: if full, cannot take item 
	
    if (!weaponTable) return false; 
    FWeaponStruct* foundWeaponStruct = weaponTable->FindRow<FWeaponStruct>(id, ""); 

    // look up
    // make weapon struct 
    // update here
	
	//generate test tag
	//FTag* SpicyTag = TagTable->FindRow<FTag>(FName(TEXT("spicy")), TEXT(""));
	if (!TagTable)
	{
		return false;
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TagTable->GetTableAsString());
	}
	FTag* Tag = TagTable->FindRow<FTag>(FName("0"), "");
	if (!Tag)
	{
		return false;
	}

	for (int i = 0; i < weaponsList.Num(); i++) {
		if (weaponsList[i].id == FName(TEXT("Empty"))) {
			weaponsList[i] = FWeaponStruct(*foundWeaponStruct); 
            weaponsList[i].comboLength = comboLength;
			AssignWeaponEffect(i,id);
			//weaponsList[i].tags.Add(*Tag);
			//weaponsList[i].attackLength = attackLength;
			//weaponsList[i].recoveryLength = recoveryLength;
            UpdatePlayerWeaponMesh(GetItemIDAt(GetCurrentWeaponIndex()), GetItemIDAt(GetNextWeaponIndex()));
            if (!gameInstance) return false; 
            UGUISubsystem* gui = gameInstance->GetSubsystem<UGUISubsystem>(); 
            if (!gui) return false; 
            gui->RaccoonGUI->SetInventoryImage(i, id, weaponsList[i].icon); 
			return true;
		}
	}

    return false;
}

void UInventorySubsystem::AssignWeaponEffect(int index, FName id)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Weapon Name: %s"), *wname.ToString()));
	if (!IsValid(TagTable)) return;
	
	if (id == "Carrot")
	{

	}
	else if (id == "Leek")
	{

	}
	else if (id == "Chili")
	{
		FTag* Tag = TagTable->FindRow<FTag>(FName("0"), "");
		weaponsList[index].tags.Add(*Tag);
	}
}

void UInventorySubsystem::RefreshInventoryWeaponEffects()
{
	for (int i = 0; i < weaponsList.Num(); ++i)
	{
		AssignWeaponEffect(i, weaponsList[i].id);
	}
}

void UInventorySubsystem::UpdatePlayerWeaponMesh(FName id, FName id2) {

    // Look up weapon id from data table
	if (!gameInstance) return;
    ARaccoon* player = Cast<ARaccoon>(gameInstance->GetWorld()->GetFirstPlayerController()->GetPawn());
    if (!player) return;

    FWeaponStruct* foundWeaponStruct = weaponTable->FindRow<FWeaponStruct>(id, "");
	
	player->comboLength = foundWeaponStruct->comboLength;
    player->UpdateWeapon(foundWeaponStruct->heldMesh);
	player->damage = foundWeaponStruct->damageAmount[0];
	player->bIsTwoHanded = (id == "Leek");
	
	FWeaponStruct* foundWeaponStruct2 = weaponTable->FindRow<FWeaponStruct>(id2, "");
	player->UpdateBackWeapon(foundWeaponStruct2->backMesh);
}

void UInventorySubsystem::RemoveItemFromInventory(const int index)
{
	weaponsList[index] = *weaponTable->FindRow<FWeaponStruct>(FName("Empty"), "");
	FWeaponStruct& weapon = weaponsList[index];
	
	// Remove weapon mesh if currently held in hand
	if (currentWeaponIndex == index) UpdatePlayerWeaponMesh(weapon.id, GetItemIDAt(GetNextWeaponIndex()));
  
	// Remove weapon icon from GUI
	if (!gameInstance) return;
	UGUISubsystem* gui = gameInstance->GetSubsystem<UGUISubsystem>(); 
	if (!gui) return; 
	gui->RaccoonGUI->SetInventoryImage(index, weapon.id, nullptr);

	// Auto move to next inventory slot
	currentWeaponIndex = (currentWeaponIndex + 1 >= weaponsList.Num()) ? 0 : currentWeaponIndex + 1;
	UpdateCurrentGUI();
}

int UInventorySubsystem::GetNextWeaponIndex() const
{
	return (currentWeaponIndex + 1 >= weaponsList.Num()) ? 0 : currentWeaponIndex + 1;
}

void UInventorySubsystem::SetMeshForAttack(bool bIsEnding)
{
	// Look up weapon id from data table
	if (!gameInstance) return;
	ARaccoon* player = Cast<ARaccoon>(gameInstance->GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!player) return;

	FWeaponStruct* foundWeaponStruct = weaponTable->FindRow<FWeaponStruct>(GetItemIDAt(currentWeaponIndex), "");
	if (!bIsEnding) player->UpdateWeapon(foundWeaponStruct->attackMesh);
	else player->UpdateWeapon(foundWeaponStruct->heldMesh);
}




// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "InventorySubsystem.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UPrimitiveComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->SetupAttachment(RootComponent);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionBox"));
	CollisionComponent->SetupAttachment(MeshComponent);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	id = WeaponTableRow.RowName; 
	
    FWeaponStruct* foundWeaponStruct = WeaponTableRow.GetRow<FWeaponStruct>(""); 

	if (!foundWeaponStruct) return; 
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("freshness %s"), *WeaponTableRow.RowName.ToString()));

	comboLength = foundWeaponStruct->comboLength; 
	
	MeshComponent->SetStaticMesh(foundWeaponStruct->attackMesh); 
	MeshComponent->SetSimulatePhysics(true);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::SetItem(FDataTableRowHandle dataTableRow)
{
	WeaponTableRow = dataTableRow;
}

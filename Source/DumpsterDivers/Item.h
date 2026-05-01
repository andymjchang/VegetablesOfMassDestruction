// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/StaticMeshComponent.h"

#include "Item.generated.h"

class UInventorySubsystem; 

UCLASS()
class DUMPSTERDIVERS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	int comboLength; 
	
	FName id;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UPrimitiveComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle WeaponTableRow; 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetItem(FDataTableRowHandle dataTableRow);

	FORCEINLINE class USphereComponent* GetSphereComponent() const { return CollisionComponent; }
    FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	
};

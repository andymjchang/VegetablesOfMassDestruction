// Fill out your copyright notice in the Description page of Project Settings.


#include "CrateComponent.h"

#include "Crate.h"
#include "CrateMeshComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
UCrateComponent::UCrateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxArmor = 10.f;
	CurrentArmor = MaxArmor;
	CrateComponentMesh = nullptr;
	OwnerUI = nullptr;
	LaunchAngleDegrees = 45.f;
	LaunchMagnitude = 400.f;
}


// Called when the game starts
void UCrateComponent::BeginPlay()
{
	Super::BeginPlay();

	CrateComponentMesh = Cast<UCrateMeshComponent>(GetOwner()->GetComponentByClass(UCrateMeshComponent::StaticClass()));
	
	UWidgetComponent* WidgetComponent = Cast<UWidgetComponent>(GetOwner()->GetComponentByClass(UWidgetComponent::StaticClass()));
	OwnerUI = (WidgetComponent) ? Cast<UEnemyUI>(WidgetComponent->GetUserWidgetObject()) : nullptr;

	if (!OwnerUI || MaxArmor <= 0.f) return;
	OwnerUI->ShowArmorBar();
	OwnerUI->UpdateArmorBar(CurrentArmor / MaxArmor);
}

void UCrateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UCrateComponent::ArmorBreak()
{	
	if (CrateComponentMesh)
	{
		FVector SpawnLocation = CrateComponentMesh->GetComponentLocation() + FVector(0.f, 0.f, 100.f);
		FRotator SpawnRotation = CrateComponentMesh->GetComponentRotation();
		ACrate* SpawnedCrate = GetWorld()->SpawnActorDeferred<ACrate>(CrateClass, FTransform(SpawnRotation, SpawnLocation));

		if (!SpawnedCrate) return;

		FVector DirectionToBreakable = (SpawnLocation - GetOwner()->GetActorLocation()).GetSafeNormal() * FVector(1.f, 1.f, 0.f);
		FVector RightVector = FVector::CrossProduct(DirectionToBreakable, FVector::UpVector).GetSafeNormal();
		FQuat QuatRotation = FQuat(RightVector, FMath::DegreesToRadians(LaunchAngleDegrees));
		FVector LaunchDirection = QuatRotation.RotateVector(DirectionToBreakable);
		FVector LaunchVelocity = LaunchDirection * LaunchMagnitude;

		FVector SpinTorque = FMath::VRand() * 100.f;

		UBoxComponent* SpawnedCrateHurtbox = SpawnedCrate->GetHurtBox();
		UStaticMeshComponent* SpawnedCrateMesh = SpawnedCrate->GetCrateMesh();
		
		SpawnedCrate->SetItemData(ItemData);
		if (SpawnedCrateHurtbox) SpawnedCrateHurtbox->SetGenerateOverlapEvents(false);
		if (SpawnedCrateMesh) SpawnedCrateMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		SpawnedCrate->FinishSpawning(FTransform(SpawnRotation, SpawnLocation));
	
		// Apply upwards force
		if (SpawnedCrateMesh) SpawnedCrateMesh->AddImpulse(LaunchVelocity, FName(TEXT("None")), true);
		if (SpawnedCrateMesh) SpawnedCrateMesh->AddTorqueInRadians(SpinTorque, FName(TEXT("None")), true);

		CrateComponentMesh->SetHiddenInGame(true);
	}
		
	if (OwnerUI) OwnerUI->HideArmorBar();
}

// Called every frame
void UCrateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCrateComponent::ProcessDamage(float DamageAmount)
{
	CurrentArmor -= DamageAmount;

	if (OwnerUI) OwnerUI->UpdateArmorBar(FMath::Clamp(CurrentArmor, 0.f, MaxArmor) / MaxArmor);

	if (CurrentArmor <= 0.f) ArmorBreak();
}

bool UCrateComponent::IsArmorActive()
{
	return CurrentArmor > 0.f;
}

void UCrateComponent::SetCrateItemData(FDataTableRowHandle NewItemData)
{
	ItemData = NewItemData;
}




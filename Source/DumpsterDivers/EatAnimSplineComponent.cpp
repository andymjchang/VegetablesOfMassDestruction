// Fill out your copyright notice in the Description page of Project Settings.


#include "EatAnimSplineComponent.h"
#include "Raccoon.h"

UEatAnimSplineComponent::UEatAnimSplineComponent()
{
	// Initialize variables
	WeaponTossSpinSpeed = 1000.f;
	IsMovingAlongSpline = false;
	SplineDistance = 0.0f;
	ElapsedTime = 0.0f;
	SplineMovementSpeed = 1.0f;
	WeaponMesh = nullptr;
	RaccoonPlayer = nullptr;
	TotalDuration = 0.0f;
	SetRelativeLocation(FVector::ZeroVector);

	// Create mesh component
	MeshCopy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshCopy"));
	MeshCopy->SetupAttachment(this);
	MeshCopy->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UEatAnimSplineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update first spline point to match position of weapon hand
	if (RaccoonPlayer) SetWorldLocationAtSplinePoint(0, RaccoonPlayer->GetMesh()->GetSocketLocation(FName("RightHandSocket")));

	if (!IsMovingAlongSpline || !MeshCopy || !RaccoonPlayer) return;

	// Calculate normalized progress along the spline (0 to 1)
	float SplineLength = GetSplineLength();
	float NormalizedProgress = FMath::Clamp(ElapsedTime / TotalDuration, 0.0f, 1.0f);

	// Apply easing function to control speed
	float EasedProgress = CubicBezier(NormalizedProgress, 0.85f, 0.20f);

	// Calculate the distance along the spline based on eased progress
	SplineDistance = EasedProgress * SplineLength;

	// Reset mesh transform & variables after completing spline
	// if (NormalizedProgress >= 1.0f)
	// {
	// 	StopSplineMovement();
	// 	return;
	// }

	// Apply easing to the location along the spline
	FVector LocationOffset = GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::Local);
	FVector RotatedOffset = RaccoonPlayer->GetActorQuat().RotateVector(LocationOffset);

	// Apply spinning movement
	float SpinAngle = ElapsedTime * WeaponTossSpinSpeed;
	FRotator SpinRotation(SpinAngle, 0.0f, 0.0f);
	FRotator FinalRotation = RaccoonPlayer->GetActorRotation() + SpinRotation;

	// Update the weapon's transform
	MeshCopy->SetWorldLocation(RaccoonPlayer->GetActorLocation() + RotatedOffset);
	MeshCopy->SetWorldRotation(FinalRotation);

	// Increment elapsed time
	ElapsedTime += DeltaTime;
}

void UEatAnimSplineComponent::StartSplineMovement(float pTotalDuration)
{
	if (pTotalDuration <= 0.0f) return;

	if (!WeaponMesh || !MeshCopy) return;

	// Reset variables
	IsMovingAlongSpline = true;
	SplineDistance = 0.0f;
	ElapsedTime = 0.0f;
	SplineMovementSpeed = GetSplineLength() / pTotalDuration;
	this->TotalDuration = pTotalDuration;

	// Copy properties from the original mesh
	MeshCopy->SetStaticMesh(WeaponMesh->GetStaticMesh());
	MeshCopy->SetWorldTransform(WeaponMesh->GetComponentTransform());

	WeaponMesh->SetHiddenInGame(true);
}

void UEatAnimSplineComponent::StopSplineMovement()
{
	IsMovingAlongSpline = false;
	MeshCopy->SetStaticMesh(nullptr);
	WeaponMesh->SetHiddenInGame(false);
}

void UEatAnimSplineComponent::SetWeaponMesh(UStaticMeshComponent* Weapon)
{
	WeaponMesh = Weapon;
}

void UEatAnimSplineComponent::SetPlayer(ARaccoon* Player)
{
	RaccoonPlayer = Player;
}

// Cubic Bézier formula:
// B(t) = (1-t)^3 * P0 + 3*(1-t)^2*t * P1 + 3*(1-t)*t^2 * P2 + t^3 * P3. (Assume P0 = 0 and P3 = 1)
float UEatAnimSplineComponent::CubicBezier(float t, float P1, float P2)
{
	return FMath::Pow(1 - t, 3) * 0 + 3 * FMath::Pow(1 - t, 2) * t * P1 + 3 * (1 - t) * FMath::Pow(t, 2) * P2 + FMath::Pow(t, 3) * 1;
}


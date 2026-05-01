// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "EatAnimSplineComponent.generated.h"

/**
 * 
 */

class ARaccoon;

UCLASS()
class DUMPSTERDIVERS_API UEatAnimSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UEatAnimSplineComponent();

	// Override TickComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimationParameters, meta = (AllowPrivateAccess = "true"))
	float WeaponTossSpinSpeed;

	UFUNCTION(BlueprintCallable)
	void StartSplineMovement(float pTotalDuration);

	UFUNCTION(BlueprintCallable)
	void StopSplineMovement();
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponMesh(UStaticMeshComponent* Weapon);

	UFUNCTION(BlueprintCallable)
	void SetPlayer(ARaccoon* RaccoonPlayer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;

	UPROPERTY()
	class UStaticMeshComponent* MeshCopy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class ARaccoon* RaccoonPlayer;

private:
	// Spline movement variables
	bool IsMovingAlongSpline;
	float SplineDistance;
	float SplineMovementSpeed;
	float ElapsedTime;
	float TotalDuration;

	static float CubicBezier(float t, float P1, float P2);
};

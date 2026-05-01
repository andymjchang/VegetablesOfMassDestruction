// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "HitFlashComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUMPSTERDIVERS_API UHitFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHitFlashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UMeshComponent* FindOwnerMeshComponent() const;
	
	FName TargetTag = FName("PrimaryMesh");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* MaterialInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMeshComponent* OwnerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	// Timers
	FTimerHandle HitFlashTimerHandle;
	FTimerHandle TelegraphFlashTimerHandle;
	
	//void StartTelegraphFlash(float Duration);
	void StartHitFlash(float Duration, UMaterialInterface* FlashMaterial);
	
	void EndHitFlash();
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	void EndTelegraphFlash() const;

	UFUNCTION()
	void OnTelegraphTick(float Output);
	
public:	
	void TelegraphFlash(float Duration);
	void HitFlash(float Duration);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* HitFlashMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* TelegraphFlashMaterial;
	
	UPROPERTY()
	UTimelineComponent* TelegraphTimeline;
	UPROPERTY(EditAnywhere)
	UCurveFloat* TelegraphCurve;
};

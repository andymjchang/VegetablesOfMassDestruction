// Fill out your copyright notice in the Description page of Project Settings.

#include "HitFlashComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Materials/MaterialInterface.h"

/**
 *  This component requires the parent to have a static mesh or skeletal mesh
 *  that is tagged with 'PrimaryMesh'
 */

// Sets default values for this component's properties
UHitFlashComponent::UHitFlashComponent(): MaterialInstance(nullptr), OwnerMesh(nullptr),
                                          DynamicMaterialInstance(nullptr), HitFlashMaterial(nullptr),
                                          TelegraphFlashMaterial(nullptr)
{
    PrimaryComponentTick.bCanEverTick = false;
    TelegraphTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TelegraphTimeline"));
}

void UHitFlashComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Detect mesh component and store the original material
    OwnerMesh = FindOwnerMeshComponent();
    if (OwnerMesh)
    {
        MaterialInstance = OwnerMesh->GetMaterial(0);
        if (MaterialInstance) DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
        if (DynamicMaterialInstance) OwnerMesh->SetMaterial(0, DynamicMaterialInstance);
    }

    // Setup Telegraph Timeline
    if (TelegraphTimeline)
    {
        FOnTimelineFloat ProgressFunction;
        ProgressFunction.BindDynamic(this, &UHitFlashComponent::OnTelegraphTick);

        TelegraphTimeline->AddInterpFloat(TelegraphCurve, ProgressFunction);
        TelegraphTimeline->SetTimelineLength(0.3f);
    }
}

UMeshComponent* UHitFlashComponent::FindOwnerMeshComponent() const
{
    if (const AActor* Owner = GetOwner())
    {
        // Search for a StaticMeshComponent
        if (UStaticMeshComponent* StaticMesh = Owner->FindComponentByTag<UStaticMeshComponent>(TargetTag))
        {
            return StaticMesh;
        }

        // Search for a SkeletalMeshComponent
        if (USkeletalMeshComponent* SkeletalMesh = Owner->FindComponentByTag<USkeletalMeshComponent>(TargetTag))
        {
            return SkeletalMesh;
        }
    }
    return nullptr;
}

void UHitFlashComponent::TelegraphFlash(float Duration)
{
    if (!OwnerMesh || !DynamicMaterialInstance) return;
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Duration));

    DynamicMaterialInstance->SetScalarParameterValue("GlowStrength", 0.f);
    TelegraphTimeline->PlayFromStart();

    GetWorld()->GetTimerManager().SetTimer(
        TelegraphFlashTimerHandle, 
        this, 
        &UHitFlashComponent::EndTelegraphFlash, 
        Duration, 
        false
    );
}

void UHitFlashComponent::EndTelegraphFlash() const
{
    if (!OwnerMesh || !DynamicMaterialInstance) return;
    DynamicMaterialInstance->SetScalarParameterValue("GlowStrength", 0.f);
}

void UHitFlashComponent::HitFlash(float Duration)
{
    StartHitFlash(Duration, HitFlashMaterial);
}

void UHitFlashComponent::StartHitFlash(float Duration, UMaterialInterface* FlashMaterial)
{
    if (!OwnerMesh || !FlashMaterial) return;
    
    OwnerMesh->SetMaterial(0, FlashMaterial);
    
    GetWorld()->GetTimerManager().SetTimer(
        HitFlashTimerHandle, 
        this, 
        &UHitFlashComponent::EndHitFlash, 
        Duration, 
        false
    );
}

void UHitFlashComponent::OnTelegraphTick(const float Output)
{
    if (DynamicMaterialInstance) DynamicMaterialInstance->SetScalarParameterValue("GlowStrength", Output);
}

void UHitFlashComponent::EndHitFlash()
{
    // Revert to the original material
    if (!OwnerMesh || !MaterialInstance || !DynamicMaterialInstance) return;
    OwnerMesh->SetMaterial(0, DynamicMaterialInstance);
}

void UHitFlashComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TelegraphFlashTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(HitFlashTimerHandle);
    }

    if (TelegraphTimeline)
    {
        TelegraphTimeline->Stop();
    }
}


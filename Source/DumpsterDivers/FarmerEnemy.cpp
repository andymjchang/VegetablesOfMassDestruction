// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmerEnemy.h"
//#include "GameFramework/CharacterMovementComponent.h"

AFarmerEnemy::AFarmerEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

}

void AFarmerEnemy::Attack()
{
    SpawnVFX(AttackVFX);
    AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
    // AttackCollisionComponent->bHiddenInGame = false;
    // AttackCollisionComponent->MarkRenderStateDirty();

    // Using Root Motion Dash
    TSharedPtr<FRootMotionSource_ConstantForce> ChargeForce = MakeShareable(new FRootMotionSource_ConstantForce());
    ChargeForce->InstanceName = FName("ChargeForce");
    ChargeForce->Priority = 5;
    ChargeForce->AccumulateMode = ERootMotionAccumulateMode::Override;
    ChargeForce->Force = GetActorForwardVector() * ChargeStrength;
    ChargeForce->Duration = ChargeDuration;
    GetCharacterMovement()->ApplyRootMotionSource(ChargeForce);

    float timerLength = AttackDuration;

    GetWorld()->GetTimerManager().SetTimer(
        AttackTimer,
        [this]()
        {
            AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            // AttackCollisionComponent->MarkRenderStateDirty();
            // AttackCollisionComponent->bHiddenInGame = true;
            FRotator rot = GetActorRotation();
            rot.Pitch = 0.0f;
            SetActorRotation(rot);
        },
        timerLength, 
        false
    );
}



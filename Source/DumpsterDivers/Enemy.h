// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatUnit.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "EnemyUI.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDefeated, AEnemy*, EnemyPtr);

UCLASS()
class DUMPSTERDIVERS_API AEnemy : public ACharacter, public ICombatUnit
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	virtual void PostInitializeComponents() override;
	float health; 
	float maxHealth;

	virtual void Telegraph(float DeltaSeconds);
	virtual void endTelegraph();
	virtual void Start_telegraph_flash(float Duration);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SectionID;

	UPROPERTY (EditAnywhere) 
	class UBehaviorTree * BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* AttackVFX;
	UPROPERTY(EditAnywhere, Category = "VFX")
	FRotator AttackRotation;
	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* HitVFX;

	UPROPERTY(EditAnywhere, Category = "Combat Data")
	float DetectionRange = 300.f;
	UPROPERTY(EditAnywhere, Category = "Combat Data")
	float ChargeStrength = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Combat Data")
	float ChargeDuration = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Combat Data")
	float AttackDuration = 0.25f;
	UPROPERTY(EditAnywhere, Category = "Combat Data")
	float KnockbackStrength = 200.f;
	UPROPERTY(EditAnywhere, Category = "Combat Data")
	float KnockbackDuration = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Combat Data")
	float BaseDamage = 10.f;

	UPROPERTY(BlueprintReadWrite)
	bool bHasLanded = true;
	
	void SpawnVFX(UNiagaraSystem* VFX);
	//CombatInterface
	virtual void TakeUnitDamage(float Damage) override;
	virtual void TickUnitDamage(float Damage, int Duration) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HurtCollisionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UHitFlashComponent* HitFlashComponent;
	//void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	TArray<FGuid> hitIDs;
	
	FTimerHandle AttackTimer;
	FTimerHandle DotTimer;//for looping each tick
	FTimerHandle EffectTimer;//for duration of the effect
	FTimerHandle KnockbackTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// UFUNCTION(BlueprintCallable, Category = "Damage")
	// void FollowPlayer(); 

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* otherComponent, int32 OtherIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
	void ProcessDamage(float DamageAmount);
	virtual void StartKnockback(const AActor* OtherActor, float StrengthScale = 1.f);
	void SetIsKnockback(bool toggle) const;

	UFUNCTION(BlueprintCallable)
	void ResumeAIMovement(int SectionNumber);

	virtual void Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* EnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* AttackCollisionComponent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyDefeated OnEnemyDefeated;
};

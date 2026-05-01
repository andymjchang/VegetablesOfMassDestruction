// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ChildActorComponent.h"
#include "GameFramework/Controller.h"
#include "RaccoonControllerInterface.h"
#include "Components/BoxComponent.h" 
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "InventorySubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/DataTable.h"
#include "StateMachineComponent.h"
#include "Sound/SoundBase.h"
#include "Components/TimelineComponent.h"
#include "EatAnimSplineComponent.h"
#include "CameraShakeComponent.h"
#include "CombatUnit.h"
#include "Companion.h"
#include "HitFlashComponent.h"
#include "Pig.h"
#include "Raccoon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEatComplete);

enum ECompanion_Type { None, Chicken, Pig };

UCLASS()
class DUMPSTERDIVERS_API ARaccoon : public ACharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ARaccoon();

	//float health;
	FGuid most_recent_hit_serialization;
	FGuid PigFGuid;
	int comboLength;

	FOnDashReady OnDashReady;
	FOnDashStart OnDashStart;
	FOnDeath OnDeath;
	FOnEatComplete OnEatComplete;
	//Stats
	int power;
	int crit_chance;
	float speed;
	float attack_speed;
	float crit_multiplier;

	// Toggles
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTwoHanded;
	bool bIsPigUnlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Companion")
	TSubclassOf<AActor> CompanionBP;

	UPROPERTY()
	APig* PigHitbox;
	
	
	
	float CurrentHealth;
	float MaxHealth;
	const float ManaCost = 100;

	// Companion gauge
	float CurrentMana;
	float MaxMana;

	// Dashing properties
	UPROPERTY()
	float BaseMaxWalkSpeed;
	ECompanion_Type CompanionType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	TArray<float> DashStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	TArray<float> DashDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	TArray<int> MaxDashCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	TArray<float> DashCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FVector CompanionDashOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	FVector CompanionTeleportOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float AttackDashStrength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float AttackDashDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float PigMaxDashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float PigDashRotationSpeed;
	
	UFUNCTION(BlueprintImplementableEvent)
	void DashReadyEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void DashStartEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void DeathEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void EatCompleteEvent();
	
	// Dash Timeline Function
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveToLocation(FVector TargetLocation, int CompanionType);
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetDashLocation(FVector Start, FVector Direction, int CompanionType);

	UFUNCTION()
	bool CheckCapsuleAtLocation(const FVector& Location, float radius, float halfHeight);
	
	UPROPERTY()
    UTimelineComponent* MovementTimeline;
	
    UPROPERTY(EditAnywhere, Category = "Movement")
    UCurveFloat* MovementCurve;
	
	UFUNCTION()
	void OnMovementTick(float Alpha);
	
	UFUNCTION()
	void OnMovementFinished();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WeaponDecayEnabled;

	// Length of time it take for a player to complete eating action
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EatTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EnableTimeSlowWhileEating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (EditCondition = "EnableTimeSlowWhileEating", ClampMin="0.01", ClampMax="1.00"))
	float TimeDilationMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemSpawnParameters, meta = (EditCondition = "EnableTimeSlowWhileEating"))
	float CameraZoomOffset;

	float OriginalOrthoWidth;

	bool attBuffer;
	int combo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EggBaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EggBaseRadius;
	
	TArray<EPlayerState> inputBuffer;

	void StartCameraShake(float duration, float intensity) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DamageNumberColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HealNumberColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CritNumberColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PlayerDamageNumberColor;
	
private:	
	FTimerHandle StateTimer;
	FTimerHandle HitStopTimer;
	FTimerHandle DashCooldownTimer;
	FTimerHandle CompanionCooldownTimer;
	FTimerHandle BufferTimer;
	FTimerHandle AttkStartTimer;

	// Dashing Variables
	FVector StartLocation;
	FVector EndLocation;
	TArray<int> CurrentDashCount;

	FTimerHandle ZoomTimerHandle;
	
	bool IsWeaponMovingAlongSpline;
	bool CameraIsZooming;
	float CameraZoomTimeElapsed;

	FVector lastLookDirection;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void StartChickenDash();
	void StartPigDash();
	void EndDash();
	void EndChickenDash();
	void EndPigDash();
	void EndDashCooldown();
	void EndCompanionCooldown();
	void StartCompanionDash();
	void EndCompanionDash();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* AttackCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* PigCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HurtCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BackWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CompanionMesh;

	// Meshes to swap in and out
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* ChickenMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* PigMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStateMachineComponent* StateMachine;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraShakeComponent* CameraShakeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UEatAnimSplineComponent* EatSplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UHitFlashComponent* HitFlashComponent;

	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* AttackVFX;

	UPROPERTY(EditAnywhere, Category = "VFX")
	class UNiagaraSystem* PoofVFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    TArray<USoundBase*> AttackSounds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EatSound;
    	
	IRaccoonControllerInterface* GetRaccoonController() const;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotationSpeed;
	
	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	UForceFeedbackEffect* HitForceFeedbackEffect;
	
	void RotateToDirection(const FVector& TargetDirection);
	void RotationCoroutine(const FVector& TargetDirection, float InterpSpeed);
	bool isRotating;

	UPROPERTY()
	AActor* CompanionActor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UBoxComponent* GetHitBox() const { return AttackCollisionComponent; }
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UStateMachineComponent* GetStateMachine() const { return StateMachine; }
	FORCEINLINE class UEatAnimSplineComponent* GetAnimationSpline() const { return EatSplineComponent; }

	UFUNCTION(BlueprintCallable)
	void MoveEvent(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void AttackEvent();
	
	UFUNCTION(BlueprintCallable)
	void EatEvent();

	void HandlePassives(const FWeaponStruct* weapon);
	
	UFUNCTION(BlueprintCallable)
	void CompleteEatEvent(const int InventoryIndex);

	UFUNCTION(BlueprintCallable)
	void DashEvent();
	
	UFUNCTION(BlueprintCallable)
	void RecoveryEvent();

	UFUNCTION(BlueprintCallable)
	void addToBuffer(EPlayerState state);

	UFUNCTION()
	void handleBufferedInput(EPlayerState state,EPlayerState oldState);

	UFUNCTION() 
	void OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *otherComponent, int32 OtherIndex, bool bFromSweep, const FHitResult &Hit);
	UFUNCTION()
	void OnHurtboxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* otherComponent,
	                  int32 OtherIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void UpdateWeapon(UStaticMesh* weaponMesh) const;

	void UpdateBackWeapon(UStaticMesh* weaponMesh) const;

	UFUNCTION(BlueprintCallable)
	void Heal(float HealAmount);
	void ProcessDamage(float DamageAmount);
	void ChangeMana(float ManaAmount);

	UFUNCTION(BlueprintCallable)
	void StartZoom();

	UFUNCTION(BlueprintCallable)
	void StopZoom();

	void CameraZoomTick();

	// void SpawnVFX(UNiagaraSystem* Effect, const FVector& SpawnLocation) const;
	AActor* SpawnActor(const TSubclassOf<AActor>& SpawnActor, const FVector& SpawnOffset = FVector::ZeroVector) const;

	// void RotateToVector()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class URaccoonGUI>  RaccoonGUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UAugmentGUI> AugmentGUIClass;
	
	void ActivateHitStop();

	UFUNCTION()
	void ResetTimeDilation();
	
	UFUNCTION()
	void PlayAttackMontage() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations")
	UAnimMontage* EatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations")
	TArray<UAnimMontage*> DashMontage;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animations")
	// UAnimMontage* ChickenDashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TSubclassOf<AActor> ChickenEggClass;

	void DisableAttackHitbox() const;

	virtual int CalcDamage() const;
	int GetEggDamage() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage Numbers")
	void SpawnDamageText(const FString& text, FVector targetPosition, FVector color);


	//const FWeaponStruct* get_current_weapon();
	void playHaptics();
	void SpawnCompanion(const TSubclassOf<AActor>& ActorClass);
	void SwitchCompanionEvent();
	void UpdateAttackWeaponMesh(bool bIsEnding);
};


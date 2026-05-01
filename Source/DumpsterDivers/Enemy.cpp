// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"

#include "CrateComponent.h"
#include "EggBomb.h"
#include "EnemyController.h"
#include "EnemyUI.h"
#include "HitFlashComponent.h"
#include "InventorySubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Pig.h"
#include "Raccoon.h"
#include "RaccoonController.h"
#include "LevelSectionsSystem/LevelManagerSubsystem.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HurtCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HurtBox"));
	HurtCollisionComponent->SetupAttachment(RootComponent);
	health = 50.0f;
	maxHealth = 50.0f;
	SectionID = 1;

	// Create Healthbar Component
	EnemyHealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	EnemyHealthBar->SetupAttachment(RootComponent);		 // Attach to enemy
	EnemyHealthBar->SetWidgetSpace(EWidgetSpace::World); // Ensure it's world space
	EnemyHealthBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyHealthBar->RequestRedraw();

	// Attack Hitbox Component
	AttackCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
	AttackCollisionComponent->SetupAttachment(RootComponent);
	AttackCollisionComponent->SetGenerateOverlapEvents(true);
	AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	hitIDs = {};
	// Hit Flash Component
	HitFlashComponent = CreateDefaultSubobject<UHitFlashComponent>(TEXT("HitFlash"));
	AddInstanceComponent(HitFlashComponent);
	
}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	HitFlashComponent = Cast<UHitFlashComponent>(GetComponentByClass(UHitFlashComponent::StaticClass()));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	HurtCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHit);

	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetVisibility(true);
		UEnemyUI* HealthBarWidget = Cast<UEnemyUI>(EnemyHealthBar->GetUserWidgetObject());
		if (HealthBarWidget) HealthBarWidget->UpdateHealthBar(health / maxHealth);
	}
	
	//FollowPlayer();
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::Destroyed) OnEnemyDefeated.Broadcast(this);

	GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
	GetWorld()->GetTimerManager().ClearTimer(DotTimer);
	GetWorld()->GetTimerManager().ClearTimer(EffectTimer);
	GetWorld()->GetTimerManager().ClearTimer(KnockbackTimer);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::SpawnVFX(UNiagaraSystem* VFX)
{
	if (!VFX) return;
	USceneComponent* RootComp = GetRootComponent();
	if (!RootComp) return;
	

	// Spawn the Niagara component
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		VFX,           // The Niagara system to spawn
		RootComp,           // Parent component to attach to
		NAME_None,          // Socket name (NAME_None for no socket)
		FVector::ZeroVector, // Location offset from parent
		AttackRotation, // Rotation offset from parent
		EAttachLocation::KeepRelativeOffset, // Location type
		true,               // Auto destroy when complete
		true,               // Auto activate on spawn
		ENCPoolMethod::None // Pooling method
	);
}

// void AEnemy::FollowPlayer()
// {
// 	AEnemyController* AIController = Cast<AEnemyController>(GetController());
// 	if (AIController)
// 	{
// 		FVector Destination = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
//
// 		// Move the AI to the player
// 		FAIMoveRequest MoveRequest;
// 		MoveRequest.SetGoalLocation(Destination);
// 		MoveRequest.SetAcceptanceRadius(50.0f); // Adjust as needed
//
// 		FNavPathSharedPtr NavPath;
// 		AIController->MoveTo(MoveRequest, &NavPath);
//
// 		// Bind to movement completion event
// 		//AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemy::OnMoveCompleted);
// 	}
// }

// void AEnemy::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
// {
// 	// Follow player again if the player is far away
// }

void AEnemy::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent *otherComponent, int32 OtherIndex, bool bFromSweep, const FHitResult &Hit)
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;
	
	if (ARaccoon* raccoon = Cast<ARaccoon>(OtherActor))
	{
		if (raccoon->GetStateMachine()->GetState() == EPlayerState::PIG)
		{
			if (hitIDs.Contains(raccoon->PigFGuid)) return;
			hitIDs.Add(raccoon->PigFGuid);
			StartKnockback(OtherActor, 2.f);
			ProcessDamage(15.f);
			raccoon->StartCameraShake(0.1f, 50);
		}
		else
		{
			if (hitIDs.Contains(raccoon->most_recent_hit_serialization)) return;
			hitIDs.Add(raccoon->most_recent_hit_serialization);
			
			TArray<FTag> tags = inventory->GetCurrentWeapon()->tags;
			for (FTag Tag : tags)
			{
				if (Tag.hit_effect)
				{
					Tag.hit_effect->Apply(TScriptInterface<ICombatUnit>(this));
					if (Tag.hit_VFX) SpawnVFX(Tag.hit_VFX); 
				}
			}
			StartKnockback(raccoon, inventory->GetCurrentWeapon()->knockbackMult);
			ProcessDamage(raccoon->CalcDamage());
		}
		
		raccoon->StartCameraShake(0.1f, 50);
		raccoon->playHaptics();
		raccoon->ChangeMana(10);
		HitFlashComponent->HitFlash(0.2f);
		SpawnVFX(HitVFX);
		return;
	}

	// Get player from controller for non player collisions
	ARaccoonController* PlayerController = Cast<ARaccoonController>(GetWorld()->GetFirstPlayerController());
	ARaccoon* Player = PlayerController ? Cast<ARaccoon>(PlayerController->GetPawn()) : nullptr;
	if (!Player) return;

	if (const AEggBomb* EggBomb = Cast<AEggBomb>(OtherActor))
	{
		Player->StartCameraShake(0.1f, 25);
		StartKnockback(OtherActor);
		ProcessDamage(Player->GetEggDamage());
		HitFlashComponent->HitFlash(0.2f);
		SpawnVFX(HitVFX);
		return;
	}

	// if (const APig* Pig = Cast<APig>(OtherActor))
	// {
	// 	if (hitIDs.Contains(Pig->PigFGuid)) return;
	// 	hitIDs.Add(Pig->PigFGuid);
	// 	Player->StartCameraShake(0.1f, 15);
	// 	StartKnockback(OtherActor, 5.f);
	// 	ProcessDamage(15.f);
	// 	HitFlashComponent->HitFlash(0.2f);
	// 	SpawnVFX(HitVFX);
	// 	return;
	// }
}

void AEnemy::ProcessDamage(float DamageAmount)
{
	ARaccoonController* PlayerController = Cast<ARaccoonController>(GetWorld()->GetFirstPlayerController());
	ARaccoon* Player = PlayerController ? Cast<ARaccoon>(PlayerController->GetPawn()) : nullptr;
	float crit_chance = Player->crit_chance;
	int Random = FMath::RandRange(0, 100);
	float final_damage = DamageAmount;
	if (crit_chance > Random)
	{
		final_damage = FMath::RoundToInt(final_damage = final_damage * Player->crit_multiplier);
		if (Player) Player->SpawnDamageText(FString::FromInt(final_damage) + "!", GetActorLocation(), FVector(255,231,124));
	}else{
		if (Player) Player->SpawnDamageText(FString::FromInt(final_damage), GetActorLocation(), FVector(150, 150, 150));
	}
	//if (Player) Player->SpawnDamageText(FString::FromInt(final_damage) + "!", GetActorLocation(), FVector(205,61,30));
	//FString text, FVector targetPosition, FColor color
	
	UCrateComponent* CrateComponent = Cast<UCrateComponent>(GetComponentByClass(UCrateComponent::StaticClass()));
	if (CrateComponent && CrateComponent->IsArmorActive())
	{
		CrateComponent->ProcessDamage(final_damage);
	}
	else
	{
		health -= DamageAmount;
		
		if (!EnemyHealthBar) return;
		UEnemyUI* HealthBarWidget = Cast<UEnemyUI>(EnemyHealthBar->GetUserWidgetObject());
		if (HealthBarWidget) HealthBarWidget->UpdateHealthBar(health / maxHealth);
		
		if (health <= 0.0f) Destroy();
	}
}

void AEnemy::StartKnockback(const AActor* OtherActor, float StrengthScale)
{
	// Calculate the direction from the enemy to the raccoon
	FVector DirectionToRaccoon = OtherActor->GetActorLocation() - GetActorLocation();
	DirectionToRaccoon.Normalize();
		
	// Invert the direction to get the opposite direction
	const FVector KnockbackDirection = -DirectionToRaccoon;

	// Using Root Motion Dash
	TSharedPtr<FRootMotionSource_ConstantForce> KnockbackForce = MakeShareable(new FRootMotionSource_ConstantForce());
	KnockbackForce->InstanceName = FName("KnockbackForce");
	KnockbackForce->Priority = 5;
	KnockbackForce->AccumulateMode = ERootMotionAccumulateMode::Override;
	KnockbackForce->Force = KnockbackDirection * KnockbackStrength * StrengthScale;
	KnockbackForce->Duration = KnockbackDuration;
	GetCharacterMovement()->ApplyRootMotionSource(KnockbackForce);

	SetIsKnockback(true);
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
	}
	GetWorld()->GetTimerManager().SetTimer(
		KnockbackTimer, 
		[this]() { SetIsKnockback(false); },
		KnockbackDuration, 
		false
	);
}

void AEnemy::SetIsKnockback(bool toggle) const
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool(FName("IsKnockback"), toggle);
		}
	}
}

void AEnemy::ResumeAIMovement(int SectionNumber)
{
	if (SectionNumber != SectionID) return;
	
	AEnemyController* AIController = Cast<AEnemyController>(GetController());
	UBrainComponent* BrainComponent = (AIController) ? AIController->GetBrainComponent() : nullptr;
	if (BrainComponent) BrainComponent->StartLogic();
}

void AEnemy::Attack()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BASE ATTK"));
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
			//FRotator rot = GetActorRotation();
			//rot.Pitch = 0.0f;
			//SetActorRotation(rot);
		},
		timerLength, 
		false
	);
}

void AEnemy::Start_telegraph_flash(float Duration)
{
	HitFlashComponent->TelegraphFlash(Duration);
}

void AEnemy::Telegraph(float DeltaSeconds)
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	if (!gameInstance) return;
	ARaccoon* player = Cast<ARaccoon>(gameInstance->GetWorld()->GetFirstPlayerController()->GetPawn());
	//Tilt the enemy back first
	float TiltAngle = 80.0f; // Adjust this value as needed
	FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation = CurrentRotation;
	TargetRotation.Pitch += TiltAngle; // Add the tilt angle to the pitch
    
	// Interpolate smoothly towards the target rotation for tilting
	float TiltInterpSpeed = 2.0f; // Speed of interpolation for tilting
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, TiltInterpSpeed);
	SetActorRotation(NewRotation);
    
	// Calculate desired horizontal rotation
	FVector DirectionToPlayer = (player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	CurrentRotation = GetActorRotation();
	TargetRotation = DirectionToPlayer.Rotation();
    
	// Interpolate horizontal rotation
	const float RotationSpeed = 5.0f; // Adjust this value to control rotation speed
	NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);
	SetActorRotation(NewRotation);
}

void AEnemy::endTelegraph()
{
	FRotator CurrentRotation = GetActorRotation();
	FRotator NewRotation = CurrentRotation;
	NewRotation.Pitch = -20.0f;
	SetActorRotation(NewRotation);
}

void AEnemy::TakeUnitDamage(float Damage)
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	if (!gameInstance) return;
	ARaccoon* player = Cast<ARaccoon>(gameInstance->GetWorld()->GetFirstPlayerController()->GetPawn());
	ProcessDamage(Damage);
}

void AEnemy::TickUnitDamage(float Damage, int Duration)
{
	float tickRate = 0.5f;

	float TimeElapsed = 0.0f;
	if (!GetWorld()->GetTimerManager().IsTimerActive(EffectTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(
			EffectTimer,
			[this]()
			{
				if (GetWorld()->GetTimerManager().IsTimerActive(DotTimer))
				{
					GetWorld()->GetTimerManager().ClearTimer(DotTimer);
				}
			},
			Duration,
			false  // Keep looping until we clear the timer
		);
	}

	if (!GetWorld()->GetTimerManager().IsTimerActive(DotTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(
			DotTimer,
			[this, Damage]()
			{
				TakeUnitDamage(Damage);
			},
			tickRate,
			true  // Keep looping until we clear the timer
		);
	}
}

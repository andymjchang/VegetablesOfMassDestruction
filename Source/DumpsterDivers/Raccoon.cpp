// Fill out your copyright notice in the Description page of Project Settings.

#include "Raccoon.h"

#include <Programs/UnrealBuildAccelerator/Core/Public/UbaBase.h>

#include "NiagaraFunctionLibrary.h"
#include "Animation/AnimMontage.h"
#include "Enemy.h"
#include "GUISubsystem.h"
#include "GameManagerInstance.h"
#include "Kismet/GameplayStatics.h"
#include "InventorySubsystem.h"
#include "Item.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "EnhancedInputComponent.h"
#include "DrawDebugHelpers.h"
#include "EggBomb.h"
#include "Projectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Debug/ReporterBase.h"
#include "Engine/LatentActionManager.h"

// Sets default values
ARaccoon::ARaccoon()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	CameraBoom->SetRelativeLocation(FVector(0, 0, 50));
	CameraBoom->bDoCollisionTest = true;

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 2;
	CameraBoom->CameraLagMaxDistance = 1.5f;

	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 4;
	CameraBoom->CameraLagMaxTimeStep = 1;

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create Camera Shake Component
	CameraShakeComponent = CreateDefaultSubobject<UCameraShakeComponent>(TEXT("CameraShake"));
	AddInstanceComponent(CameraShakeComponent);

	// Create State Machine
	StateMachine = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));
	AddInstanceComponent(StateMachine);

	// Controller settings
	bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

	// Setup Weapon
	CurrentWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CurrentWeapon"));
	CurrentWeapon->SetupAttachment(GetMesh(), FName(TEXT("RightHandSocket")));

	// Setup Weapon
	BackWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackWeapon"));
	BackWeapon->SetupAttachment(GetMesh(), FName(TEXT("SpineSocket")));

	// Setup Companion 
	CompanionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CompanionMesh"));
	CompanionMesh->SetupAttachment(GetMesh());
	PigFGuid = FGuid::NewGuid();
	
	// Setup Dash
	MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MovementTimeline"));
	MaxDashCount.Init(1, 3);
	DashStrength.Init(400.f, 3);
	DashDuration.Init(0.2f, 3);
	DashCooldown.Init(1.f, 3);
	CurrentDashCount.Init(1.f, 3);

	// Create spline component for eating animation
	EatSplineComponent = CreateDefaultSubobject<UEatAnimSplineComponent>(TEXT("SplineComponent"));
	EatSplineComponent->SetupAttachment(GetCapsuleComponent(), FName(TEXT("None")));
	EatSplineComponent->SetWeaponMesh(CurrentWeapon);
	EatSplineComponent->PrimaryComponentTick.bCanEverTick = true;
	EatSplineComponent->PrimaryComponentTick.bStartWithTickEnabled = true;
	// EatSplineComponent->RegisterComponent();


	CurrentHealth = 100;
	MaxHealth = 100;
	CurrentMana = 0;
	MaxMana = 100;
	power = 5;
	crit_chance = 10;
	speed = 1;
	attack_speed = 1;
	crit_multiplier = 1.5;
	
	WeaponDecayEnabled = false;
	EatTime = 0.75f;
	IsWeaponMovingAlongSpline = false;
	EnableTimeSlowWhileEating = true;
	TimeDilationMultiplier = 0.2f;
	CameraZoomOffset = 400.f;
	CameraIsZooming = false;
	CameraZoomTimeElapsed = 0.f;

	attBuffer = false; 
	combo = 0; // number reflects what swing we are one, starting from 0

	isRotating = false;

	AttackCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
	AttackCollisionComponent->SetupAttachment(RootComponent);
	AttackCollisionComponent->SetGenerateOverlapEvents(true);

	PigCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PigCollision"));
	PigCollisionComponent->SetupAttachment(RootComponent);
	PigCollisionComponent->SetGenerateOverlapEvents(true);
	
	HurtCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HurtCollision"));
	HurtCollisionComponent->SetupAttachment(RootComponent);
	HurtCollisionComponent->SetGenerateOverlapEvents(true); 
}

// Called when the game starts or when spawned
void ARaccoon::BeginPlay()
{
	Super::BeginPlay();

	// Temporary, should be none
	CompanionType = Chicken;

	// Camera zoom level
	OriginalOrthoWidth = (CameraComponent) ? CameraComponent->OrthoWidth : 0.f;
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARaccoon::OnHit);
	OnDashReady.AddDynamic(this, &ARaccoon::DashReadyEvent);
	OnDashStart.AddDynamic(this, &ARaccoon::DashStartEvent);
	OnDeath.AddDynamic(this, &ARaccoon::DeathEvent);
	OnEatComplete.AddDynamic(this, &ARaccoon::EatCompleteEvent);
	OnEatComplete.AddDynamic(EatSplineComponent, &UEatAnimSplineComponent::StopSplineMovement);
	
	EatSplineComponent->SetPlayer(this);
	inputBuffer = TArray<EPlayerState>();

	most_recent_hit_serialization = FGuid::NewGuid();
	
	// Access Subsystems
	if (const UGameManagerInstance* GameInstance = Cast<UGameManagerInstance>(GetGameInstance()))
	{
		if (UGUISubsystem* GUI = GameInstance->GetSubsystem<UGUISubsystem>())
		{
			if (URaccoonGUI* RaccoonGUI = CreateWidget<URaccoonGUI>(GetWorld(), RaccoonGUIClass))
			{
				RaccoonGUI->AddToViewport();
				GUI->RaccoonGUI = RaccoonGUI;
				GUI->RaccoonGUI->UpdateHealthBar(CurrentHealth, MaxHealth);
			}

			if (UAugmentGUI* AugmentGUI = CreateWidget<UAugmentGUI>(GetWorld(), AugmentGUIClass))
			{
				AugmentGUI->AddToViewport();
				AugmentGUI->SetVisible(false);
				GUI->AugmentGUI = AugmentGUI;
				GUI->AugmentGUI->TemporaryCompanionSetup();
			}
		}
	}

	// Setup Dash Timeline
	if (MovementCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindDynamic(this, &ARaccoon::OnMovementTick);

		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindDynamic(this, &ARaccoon::OnMovementFinished);

		MovementTimeline->AddInterpFloat(MovementCurve, ProgressFunction);
		MovementTimeline->SetTimelineFinishedFunc(FinishedFunction);
		MovementTimeline->SetTimelineLength(1.0f);
	}
	CurrentDashCount[CompanionType] = MaxDashCount[CompanionType];
	
	// Setup Camera Shake
	CameraShakeComponent->InitializeCameraShake(CameraComponent);

	// Setup Hurtbox
	HurtCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARaccoon::OnHurtboxHit);

	// Store original movement speed
	BaseMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// Setup hitflash component
	HitFlashComponent = Cast<UHitFlashComponent>(GetComponentByClass(UHitFlashComponent::StaticClass()));
	
}

void ARaccoon::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(StateTimer);
		TimerManager.ClearTimer(HitStopTimer);
		TimerManager.ClearTimer(DashCooldownTimer);
		TimerManager.ClearTimer(BufferTimer);
		TimerManager.ClearTimer(ZoomTimerHandle);
		TimerManager.ClearTimer(CompanionCooldownTimer);
		TimerManager.ClearTimer(AttkStartTimer);
	}
}

// Called every frame
void ARaccoon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraIsZooming)
	{
		CameraZoomTimeElapsed += DeltaTime;
		CameraZoomTick();
	}

	if (StateMachine->GetState() == EPlayerState::PIG)
	{
		const FVector CurrentForward = GetActorForwardVector();
		AddMovementInput(CurrentForward, 1.0f);
	}
}

// Called to bind functionality to input
void ARaccoon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

IRaccoonControllerInterface* ARaccoon::GetRaccoonController() const
{
    return Cast<IRaccoonControllerInterface>(GetController());
}

void ARaccoon::MoveEvent(const FInputActionValue& Value) {

	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller == nullptr) return;
	
	// find out which way is forward
	const FRotator Rotation = CameraComponent->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	lastLookDirection = FVector(MovementVector.X, MovementVector.Y, 0.f).RotateAngleAxis(45.f, FVector::UpVector);
	
	if (StateMachine->GetState() == EPlayerState::RIDLE)
	{
	    AddMovementInput(ForwardDirection, MovementVector.X);
	    AddMovementInput(RightDirection, MovementVector.Y);
	}
	if (StateMachine->GetState() == EPlayerState::PIG)
	{
		// Calculate the desired direction based on input
		FVector DesiredDirection = ForwardDirection * MovementVector.X + RightDirection * MovementVector.Y;
    
		// If there's input, normalize the desired direction
		if (!DesiredDirection.IsNearlyZero())
		{
			DesiredDirection.Normalize();
        
			// Calculate rotation to gradually turn towards the input direction
			FRotator TargetRotation = DesiredDirection.Rotation();
			FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), PigDashRotationSpeed);
			
			SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
		}
	}
}

bool operator==(const FVector& Lhs, FVector(* RHS)());

void ARaccoon::DashEvent()
{
	// Decide whether to use mana or not
	const bool bIsUsingMana = (CurrentMana >= ManaCost);
	
	if (bIsUsingMana == false && CurrentDashCount[None] <= 0) return;
	if (!StateMachine->ChangeState(EPlayerState::DASHING)) return;
		
	// Handle dashing direction
	FVector DashDirection = lastLookDirection.GetSafeNormal();
	if (DashDirection == FVector::ZeroVector) DashDirection = GetActorForwardVector();
	SetActorRotation(DashDirection.Rotation());
	
	// Get Anim Instance to play montages
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.f);

	OnDashStart.Broadcast();

	// Normal Dash
	if (CompanionType == None || !bIsUsingMana)
	{
		const FVector DashLocation = GetDashLocation(GetActorLocation(), DashDirection, None);
		MoveToLocation(DashLocation, None);
		CurrentDashCount[None]--;
		if (DashMontage[None]) AnimInstance->Montage_Play(DashMontage[None]);
		GetWorld()->GetTimerManager().SetTimer(
			StateTimer, 
			[this]() { EndDash(); },
			DashDuration[None], 
			false 
		);
		GetWorld()->GetTimerManager().SetTimer(
			DashCooldownTimer, 
			[this] { EndDashCooldown(); },
			DashCooldown[None], 
			false 
		);
	}
	else if (CompanionType == Chicken)
	{
		const FVector DashLocation = GetDashLocation(GetActorLocation(), DashDirection, CompanionType);
		MoveToLocation(DashLocation, CompanionType);
		if (DashMontage[Chicken]) AnimInstance->Montage_Play(DashMontage[Chicken]);
		GetWorld()->GetTimerManager().SetTimer(
			StateTimer, 
			[this]() { EndCompanionDash(); },
			DashDuration[Chicken], 
			false 
		);
		StartCompanionDash();
	}
	else if (CompanionType == Pig)
	{
		if (DashMontage[Pig]) AnimInstance->Montage_Play(DashMontage[Pig]);
		GetWorld()->GetTimerManager().SetTimer(
			StateTimer, 
			[this]() { EndCompanionDash(); },
			DashDuration[CompanionType], 
			false 
		);
		StartCompanionDash();
	}
}

void ARaccoon::StartChickenDash()
{
	AActor* EggBomb = SpawnActor(ChickenEggClass);
	if (Cast<AEggBomb>(EggBomb)) Cast<AEggBomb>(EggBomb)->SetHitboxRadius(EggBaseRadius);
	if (ChickenMesh) CompanionMesh->SetStaticMesh(ChickenMesh);
}

void ARaccoon::StartPigDash()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	StateMachine->ChangeState(EPlayerState::PIG);
	GetCharacterMovement()->MaxWalkSpeed = PigMaxDashSpeed;
	PigCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PigFGuid = FGuid::NewGuid();
	if (PigMesh) CompanionMesh->SetStaticMesh(PigMesh);
}

void ARaccoon::StartCompanionDash()
{
	ChangeMana(-ManaCost);
	if (GetMesh()) GetMesh()->AddLocalOffset(CompanionDashOffset);
	if (CompanionMesh) CompanionMesh->SetVisibility(true);
	if (CompanionActor) CompanionActor->SetActorHiddenInGame(true);
	if (const ACompanion* Companion = Cast<ACompanion>(CompanionActor)) Companion->SpawnPoofEffect();

	if (CompanionType == Chicken) StartChickenDash();
	if (CompanionType == Pig) StartPigDash();
}

void ARaccoon::EndDash()
{
	StateMachine->ChangeState(EPlayerState::RIDLE);
	GetMesh()->SetPlayRate(1.f);
}

void ARaccoon::EndChickenDash()
{

}

void ARaccoon::EndPigDash()
{
	PigCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARaccoon::EndCompanionDash()
{
	EndDash();

	// Set companion locations
	if (GetMesh()) GetMesh()->AddLocalOffset(CompanionDashOffset * -1.f);
	if (CompanionMesh) CompanionMesh->SetVisibility(false);
	FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(CompanionTeleportOffset);
	if (CompanionActor)
	{
		CompanionActor->SetActorLocation(SpawnLocation);
		CompanionActor->SetActorHiddenInGame(false);
	}

	if (CompanionType == Chicken) EndChickenDash();
	if (CompanionType == Pig) EndPigDash();
}


void ARaccoon::EndDashCooldown()
{
	CurrentDashCount[None] = MaxDashCount[None];
	OnDashReady.Broadcast();
}

void ARaccoon::EndCompanionCooldown()
{
	OnDashReady.Broadcast();
}

void ARaccoon::AttackEvent()
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;
	check(inventory);
	most_recent_hit_serialization = FGuid::NewGuid();
	
	if (combo >= comboLength)
	{
		return;
	}

	if (!StateMachine->ChangeState(EPlayerState::ATTACKING))
	{
		return;
	}
	
	if (combo < comboLength)
	{
		combo++;
	}
	if (AttackSounds.Num() > 0)
	{
		if (USoundBase* AttackSound = AttackSounds[0])
		{
			UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
		}
	}
	
	int ComboCopy = combo;
	TArray<float> attackLengthArray = inventory->GetCurrentWeapon()->attackLength;
	if (ComboCopy > attackLengthArray.Num()) ComboCopy = attackLengthArray.Num();
	float attackLength = attackLengthArray[ComboCopy - 1]; // combo is 1 indexed, array is 0 indexed
	TArray<float> damageAmountArray = inventory->GetCurrentWeapon()->damageAmount;
	if (ComboCopy > damageAmountArray.Num()) ComboCopy = damageAmountArray.Num();
	damage = damageAmountArray[ComboCopy - 1];
	
	PlayAttackMontage();
	UpdateAttackWeaponMesh(false);
	
	// AttackCollisionComponent->ShapeColor = FColor::Red;
	// AttackCollisionComponent->bHiddenInGame = false;
	// AttackCollisionComponent->MarkRenderStateDirty();
	
	FVector dashDirection = lastLookDirection;
	// Rotate player towards dash direction
	const FRotator DashRotation = dashDirection.Rotation();
	SetActorRotation(DashRotation);
	
	// Using Root Motion
	TSharedPtr<FRootMotionSource_ConstantForce> DashForce = MakeShareable(new FRootMotionSource_ConstantForce());
	DashForce->InstanceName = FName("DashForce");
	DashForce->Priority = 5;
	DashForce->AccumulateMode = ERootMotionAccumulateMode::Override;
	DashForce->Force = dashDirection * AttackDashStrength;
	DashForce->Duration = AttackDashDuration;
	GetCharacterMovement()->ApplyRootMotionSource(DashForce);
	
	//float length = 2000.0f;
	//FVector dashLocation = GetDashLocation(GetActorLocation(), dashDirection, dashStrength/2.0f);
	//MoveToLocation(dashLocation);
	//LaunchCharacter(dashDirection * length, true, true);
	GetWorld()->GetTimerManager().SetTimer(
			AttkStartTimer, 
			[this]()
			{
				if (bIsTwoHanded)
				{
					FVector twohandedExtent = FVector(48.0f, 48.0f, 32.0f);
					AttackCollisionComponent->SetBoxExtent(twohandedExtent);
					FVector WorldOffset = GetActorLocation() + GetActorForwardVector() * 210.0f;
					AttackCollisionComponent->SetWorldLocation(WorldOffset);
				}else
				{
					FVector oneHandedExtent = FVector(32.0f, 32.0f, 32.0f);
					AttackCollisionComponent->SetBoxExtent(oneHandedExtent);
					FVector WorldOffset = GetActorLocation() + GetActorForwardVector() * 140.0f;
					AttackCollisionComponent->SetWorldLocation(WorldOffset);
				}
				AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				AttackCollisionComponent->bHiddenInGame = false;
				AttackCollisionComponent->MarkRenderStateDirty();
			},
			attackLength/3, 
			false 
		);
	GetWorld()->GetTimerManager().SetTimer(
		StateTimer, 
		[this]()
		{
			AttackCollisionComponent->bHiddenInGame = true;
			AttackCollisionComponent->MarkRenderStateDirty();
			this->RecoveryEvent();
		},
		attackLength, 
		false 
	);
}


void ARaccoon::EatEvent()
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;

	if (!inventory) return;

	int InventoryIndex = inventory->GetCurrentWeaponIndex();
	if (inventory->GetItemIDAt(InventoryIndex) == FName("Empty")) return;
	if (!StateMachine->ChangeState(EPlayerState::EATING)) return;

	// Play eating montage
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	animInstance->StopAllMontages(0.f);
	animInstance->Montage_Play(EatMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f ,true);
	
	// Apply time slow effect to world, exempt player
	if (EnableTimeSlowWhileEating && TimeDilationMultiplier > 0.0f)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilationMultiplier);
		CustomTimeDilation = 1.0f / TimeDilationMultiplier;
	}

	// Zoom camera in
	StartZoom();
	
	GetWorld()->GetTimerManager().SetTimer(
		StateTimer, 
		[this, InventoryIndex]()
		{
			CompleteEatEvent(InventoryIndex);
		},
		EatTime * TimeDilationMultiplier, 
		false
	);
}

void ARaccoon::HandlePassives(const FWeaponStruct* weapon)
{
	FName wname = weapon->id;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Weapon Name: %s"), *wname.ToString()));
	if (wname == "Carrot")
	{
		crit_chance += 5;
	}
	else if (wname == "Leek")
	{
		power += 1;
	}
	else if (wname == "Chili")
	{
		GetCharacterMovement()->MaxWalkSpeed += 150;
	}
}

void ARaccoon::CompleteEatEvent(const int InventoryIndex)
{
	if (!GetStateMachine()->ChangeState(EPlayerState::RIDLE)) return;
	
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;

	if (!inventory) return;

	FName WeaponID = inventory->GetItemIDAt(InventoryIndex);
	HandlePassives(inventory->GetCurrentWeapon());
	if (WeaponID == "Carrot")
	{
		("Crit Chance Up!", GetActorLocation(), DamageNumberColor);
	}
	else if (WeaponID == "Leek")
	{
		("Attack Power Up!", GetActorLocation(), DamageNumberColor);
	}
	else if (WeaponID == "Chili")
	{
		SpawnDamageText("Movement Speed Up!", GetActorLocation(), DamageNumberColor);
	}

	// TODO: Pull heal value + other effects from data table. Hardcoded for now.
	
	float HealAmount = 30.f;
	Heal(HealAmount);
	SpawnDamageText("+" + FString::FromInt(HealAmount), GetActorLocation(), HealNumberColor);

	ChangeMana(MaxMana);

	// Play eat sound vfx
	if (EatSound) UGameplayStatics::PlaySoundAtLocation(this, EatSound, GetActorLocation(), 1.f, 1.f, 0.25f);

	// Consume item from inventory
	inventory->RemoveItemFromInventory(InventoryIndex);
	OnEatComplete.Broadcast();
}

void ARaccoon::RecoveryEvent()
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;
	check(inventory);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("AttL :%f"),inventory->GetCurrentWeapon()->attackLength));

	// Check recovery length from inventory
	int ComboCopy = combo;
	TArray<float> recoveryLengthArray = inventory->GetCurrentWeapon()->recoveryLength;
	if (ComboCopy > recoveryLengthArray.Num()) ComboCopy = recoveryLengthArray.Num();
	float recoveryLength = recoveryLengthArray[ComboCopy - 1];
	
	if (!StateMachine->ChangeState(EPlayerState::RECOVERY)) return;

	// Disable hitbox
	AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// AttackCollisionComponent->ShapeColor = FColor::Green;
	// AttackCollisionComponent->MarkRenderStateDirty();
	
	GetWorld()->GetTimerManager().SetTimer(
		StateTimer,
		[this]()
		{
			combo = 0;
			GetStateMachine()->ChangeState(EPlayerState::RIDLE);
			// AttackCollisionComponent->bHiddenInGame = true;
			// AttackCollisionComponent->MarkRenderStateDirty();
			GetMesh()->GetAnimInstance()->StopAllMontages(0.0f);
			UpdateAttackWeaponMesh(true);
		},
		recoveryLength, 
		false
	);
}

void ARaccoon::PlayAttackMontage() const
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	UInventorySubsystem* inventory = (gameInstance) ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;
	UAnimMontage* montage = (inventory) ? inventory->weaponsList[inventory->currentWeaponIndex].AttackMontage : nullptr;
	if (!montage) return;
	
	// Get the Character's AnimInstance
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (!animInstance) return;
	
	animInstance->Montage_Play(montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f ,true);
	FString montageTagName = FString("Attack") + FString::FromInt(combo);
	animInstance->Montage_JumpToSection(FName(montageTagName));
	
}

void ARaccoon::DisableAttackHitbox() const
{
	// AttackCollisionComponent->Deactivate();
	AttackCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


// destroy items on ground
void ARaccoon::OnHit(UPrimitiveComponent *HitComponent, AActor *OtherActor, UPrimitiveComponent 
	*otherComponent, int32 OtherIndex, bool bFromSweep, const FHitResult &Hit) {

	if (OtherActor->IsA(AItem::StaticClass())) {

		UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
		if (!gameInstance) return;
		UInventorySubsystem* inventory = gameInstance->GetSubsystem<UInventorySubsystem>(); 
		if (!inventory) return;
		AItem* weapon = Cast<AItem>(OtherActor); 
		if (!weapon) return;

		if (inventory->AddNewItem(weapon->id, weapon->comboLength)) {
			OtherActor->Destroy();
		}
	}
}

// Collision with enemy attack hitboxes
void ARaccoon::OnHurtboxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent
	* otherComponent, int32 OtherIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (StateMachine->GetState() == EPlayerState::DASHING) return; // iframes
	if (AEnemy* EnemyActor = Cast<AEnemy>(OtherActor))
	{
		ProcessDamage(EnemyActor->BaseDamage);
		SpawnDamageText(FString::FromInt(EnemyActor->BaseDamage), GetActorLocation(), PlayerDamageNumberColor);
		CameraShakeComponent->StartShake(0.1f, 70.f, 70.f, 0.f);
		if (HitFlashComponent) HitFlashComponent->HitFlash(0.2f);
	}
	else if (OtherActor->IsA(AProjectile::StaticClass()))
	{
		OtherActor->Destroy();
		ProcessDamage(5.f);
		SpawnDamageText(FString::FromInt(5), GetActorLocation(), PlayerDamageNumberColor);
		CameraShakeComponent->StartShake(0.1f, 50.f, 50.f, 0.f);
		if (HitFlashComponent) HitFlashComponent->HitFlash(0.2f);
	}
}

void ARaccoon::UpdateWeapon(UStaticMesh* weaponMesh) const
{
	CurrentWeapon->SetStaticMesh(weaponMesh);
	
}

void ARaccoon::UpdateBackWeapon(UStaticMesh* weaponMesh) const
{
	BackWeapon->SetStaticMesh(weaponMesh);
}

void ARaccoon::Heal(float HealAmount)
{
	CurrentHealth = FMath::Min(CurrentHealth + HealAmount, MaxHealth);

	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
	UGUISubsystem* gui = gameInstance ? gameInstance->GetSubsystem<UGUISubsystem>() : nullptr; 
	if (!gui) return;

	gui->RaccoonGUI->UpdateHealthBar(CurrentHealth, MaxHealth);
}

void ARaccoon::ProcessDamage(const float DamageAmount)
{
	CurrentHealth -= DamageAmount;
	playHaptics();
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
	if (!gameInstance) return;
	UGUISubsystem* gui = gameInstance->GetSubsystem<UGUISubsystem>(); 
	if (!gui) return;
	gui->RaccoonGUI->UpdateHealthBar(CurrentHealth, MaxHealth);
	if (CurrentHealth <= 0 && !StateMachine->ChangeState(EPlayerState::DEATH))
	{
		GetMesh()->SetSimulatePhysics(true);
		HurtCollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OnDeath.Broadcast();
	}
}

void ARaccoon::ChangeMana(const float ManaAmount)
{
	CurrentMana = FMath::Clamp(CurrentMana + ManaAmount, 0, MaxMana);
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
	if (!gameInstance) return;
	UGUISubsystem* gui = gameInstance->GetSubsystem<UGUISubsystem>(); 
	if (!gui) return;
	gui->RaccoonGUI->UpdateManaBar(CurrentMana, MaxMana);
}

void ARaccoon::CameraZoomTick()
{
	if (!CameraComponent) return;
	
	float Alpha = FMath::Clamp(CameraZoomTimeElapsed / (EatTime), 0.0f, 2.0f);
	if (Alpha >  0.f && Alpha <= 1.f) CameraComponent->OrthoWidth = FMath::Lerp(CameraComponent->OrthoWidth, OriginalOrthoWidth - CameraZoomOffset, Alpha);
	if (Alpha >  1.f && Alpha <= 2.f) CameraComponent->OrthoWidth = FMath::Lerp(CameraComponent->OrthoWidth, OriginalOrthoWidth, Alpha - 1.f);
	if (Alpha > 2.f) CameraIsZooming = false;
	
}

void ARaccoon::StartZoom()
{
	CameraIsZooming = true;
	CameraZoomTimeElapsed = 0.0f;
}

// Undo zoom when eating action cancelled. Variables reset in CameraZoomTick()
void ARaccoon::StopZoom()
{
	CameraZoomTimeElapsed = 1.0f;
	CameraIsZooming = true;
}

AActor* ARaccoon::SpawnActor(const TSubclassOf<AActor>& SpawnActor, const FVector& SpawnOffset) const
{
	if (!GetWorld() || !SpawnActor) return nullptr;
	FVector SpawnLocation = GetActorLocation() + SpawnOffset;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParameters;

	return GetWorld()->SpawnActor<AActor>(SpawnActor, SpawnLocation, SpawnRotation, SpawnParameters);
}

void ARaccoon::ActivateHitStop()
{
	CustomTimeDilation = 0.0f;
    
	// Directly set the mesh's global animation rate scale
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->GlobalAnimRateScale = CustomTimeDilation;
	}
    
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, "ResetTimeDilation");
    
	GetWorld()->GetTimerManager().SetTimer(
		HitStopTimer,
		TimerDelegate,
		0.2f,
		false
	);
}

void ARaccoon::ResetTimeDilation()
{
	CustomTimeDilation = 1.0f;
    
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->GlobalAnimRateScale = CustomTimeDilation;
	}
}

void ARaccoon::MoveToLocation(const FVector TargetLocation, int CompanionIndex)
{
	StartLocation = GetActorLocation();
	EndLocation = TargetLocation;
	// EndLocation.Z = StartLocation.Z;
	MovementTimeline->SetPlayRate(1.0f / DashDuration[CompanionIndex]);
	MovementTimeline->PlayFromStart();
}

void ARaccoon::OnMovementTick(float Alpha)
{
	// Lerp between start and end locations
	FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	SetActorLocation(NewLocation, false);  // true enables sweep
}

void ARaccoon::OnMovementFinished()
{
	// bIsMoving = false;
}

void ARaccoon::StartCameraShake(float duration, float intensity) const
{
	CameraShakeComponent->StartShake(duration, intensity, intensity, 0.f);
}

FVector ARaccoon::GetDashLocation(const FVector Start, const FVector Direction, int CompanionIndex)
{
	const float CapsuleWidth = GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float CapsuleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const int CapsuleCount = floor(DashStrength[CompanionIndex] / CapsuleWidth);

	for (int i = CapsuleCount; i > 0; i--)
	{
		FVector TargetLocation = Start + i * CapsuleWidth * Direction;
		if (CheckCapsuleAtLocation(TargetLocation, CapsuleWidth, CapsuleHeight))
		{
			return TargetLocation;
		}
	}
	return Start;
}

bool ARaccoon::CheckCapsuleAtLocation(const FVector& Location, float Radius, float HalfHeight)
{
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bHasOverlap = UKismetSystemLibrary::CapsuleOverlapActors(
		GetWorld(),
		Location,
		Radius,
		HalfHeight / 4,
		ObjectTypes,
		nullptr,
		ActorsToIgnore,
		OverlappingActors
	);

	return !bHasOverlap; // Return true if no collision found
}
//handles buffered inputs
void ARaccoon::handleBufferedInput(EPlayerState state, EPlayerState oldState)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("oldState: %d"), oldState));
	switch (state)
	{
		case(EPlayerState::RIDLE):
			return; //turn character?
		case(EPlayerState::DASHING):
			DashEvent();
			return;
		case(EPlayerState::ATTACKING):
			if (combo < comboLength)
			{
				AttackEvent();	
			}
			return;
		default:
			return;
	}
}


void ARaccoon::addToBuffer(EPlayerState state)
{
	inputBuffer.Push(state);
	if (inputBuffer.Num() == 1)
	{
		GetWorld()->GetTimerManager().SetTimer(
		
		BufferTimer, 
		[this]()
		{
			//combo = 0;
			//inputBuffer.Empty();
			//not needed because inputbuffer gets cleared anyway when state happens
		},
		2.0f, 
		false 
		);
	}
}

// void ARaccoon::RotateToDirection(const FVector& TargetDirection)
// {
// 	isRotating = true;
// 	RotationCoroutine(TargetDirection, RotationSpeed);
// }

// void ARaccoon::RotationCoroutine(const FVector& TargetDirection, float InterpSpeed)
// {
// 	// Get world subsystem for latent actions
// 	UGameInstance* GameInstance = GetGameInstance();
// 	if (!GameInstance) return;
//     
// 	FLatentActionManager& LatentActionManager = GameInstance->GetLatentActionManager();
//     
// 	// Create a custom latent action
// 	FLatentActionInfo LatentInfo;
// 	LatentInfo.CallbackTarget = this;
// 	LatentInfo.ExecutionFunction = "RotationCoroutine";  // Name of this function
// 	LatentInfo.Linkage = 0;
// 	LatentInfo.UUID = FMath::Rand();  // Unique ID for this action
//     
// 	// Loop until rotation is complete
// 	while (isRotating)
// 	{
// 		FVector NormalizedDirection = TargetDirection.GetSafeNormal();
// 		FRotator TargetRotation = NormalizedDirection.Rotation();
// 		FRotator CurrentRotation = GetActorRotation();
//
// 		constexpr float RotationThreshold = 0.01f;
// 		if (!CurrentRotation.Equals(TargetRotation, RotationThreshold))
// 		{
// 			// Still need to rotate
// 			FRotator NewRotation = FMath::RInterpTo(
// 				CurrentRotation,
// 				TargetRotation,
// 				GetWorld()->GetDeltaSeconds(),
// 				InterpSpeed
// 			);
//             
// 			SetActorRotation(NewRotation);
//             
// 			// Wait for next frame
// 			UKismetSystemLibrary::Delay(GetWorld(), 0.0f, LatentInfo);
// 		}
// 		else
// 		{
// 			// Reached target rotation, stop coroutine
// 			SetActorRotation(TargetDirection.Rotation());
// 			isRotating = false;
// 		}
// 	}
// }

// const FWeaponStruct* ARaccoon::get_current_weapon()
// {
// 	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
// 	UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;
// 	return inventory->GetCurrentWeapon();
// }

int ARaccoon::CalcDamage() const
{
	//UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	//UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;
	//const FWeaponStruct* weapon = inventory->GetCurrentWeapon();
	float final_damage = power * damage;
	
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("damage%f"), final_damage));
	return final_damage;
}

int ARaccoon::GetEggDamage() const
{
	return EggBaseDamage;
}

void ARaccoon::playHaptics()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("haptic")));
		PC->ClientPlayForceFeedback(HitForceFeedbackEffect);
	}else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("notcontroller")));
	}
}

void ARaccoon::SpawnCompanion(const TSubclassOf<AActor>& ActorClass)
{
	CompanionActor = SpawnActor(ActorClass, GetActorLocation() + CompanionDashOffset);
	CompanionType = Chicken;
}

void ARaccoon::SwitchCompanionEvent()
{
	if (CompanionType == Chicken)
	{
		CompanionType = Pig;
		
	}
	else if (CompanionType == Pig)
	{
		CompanionType = Chicken;
	}
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance()); 
	UGUISubsystem* gui = gameInstance ? gameInstance->GetSubsystem<UGUISubsystem>() : nullptr; 
	if (!gui) return;

	gui->RaccoonGUI->HighlightCompanion(CompanionType);
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Companion type: %d"), CompanionType));
}

void ARaccoon::UpdateAttackWeaponMesh(bool bIsEnding)
{
	UGameManagerInstance* gameInstance = Cast<UGameManagerInstance>(GetGameInstance());
	UInventorySubsystem* inventory = gameInstance ? gameInstance->GetSubsystem<UInventorySubsystem>() : nullptr;

	if (inventory) inventory->SetMeshForAttack(bIsEnding);
}


 
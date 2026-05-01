#include "CameraShakeComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UCameraShakeComponent::UCameraShakeComponent(): InitialLocation(), InitialRotation(), CameraBoom(nullptr)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCameraShakeComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCameraShakeComponent::StartShake(float Duration, float IntensityX, float IntensityY, float IntensityRotZ)
{
    // Set custom intensities
    ShakeIntensityX = IntensityX;
    ShakeIntensityY = IntensityY;
    ShakeIntensityRotZ = IntensityRotZ;

    ShakeDuration = Duration;
    TimeElapsed = 0.0f;

    // Start shake effect
    GetWorld()->GetTimerManager().SetTimer(ShakeTimerHandle, this, &UCameraShakeComponent::ApplyShake, ShakeFrequency, true);

    // Stop shake after duration
    GetWorld()->GetTimerManager().SetTimerForNextTick([this, Duration]()
    {
        GetWorld()->GetTimerManager().SetTimer(StopShakeTimerHandle, this, &UCameraShakeComponent::StopShake, Duration, false);
    });
}

void UCameraShakeComponent::InitializeCameraShake(USceneComponent* CameraBoomInstance)
{
    CameraBoom = CameraBoomInstance;
    InitialLocation = CameraBoom->GetRelativeLocation();
    InitialRotation = CameraBoom->GetRelativeRotation();
}

void UCameraShakeComponent::ApplyShake()
{
    if (!CameraBoom) return;
    
    TimeElapsed += ShakeFrequency; // Increment time for Perlin noise input
    
    // Calculate a decay factor that goes from 1.0 to 0.0 over the shake duration
    float Progress = FMath::Min(TimeElapsed / ShakeDuration, 1.0f);
    // float DecayFactor = 1.0f - Progress; // Linear decay
    // float DecayFactor = FMath::Cos(Progress * PI * 0.5f); // Cosine curve (starts fast, ends slow)
    float DecayFactor = 1.0f - (Progress * Progress); // Quadratic curve
    
    // Generate smooth noise values for different axes
    float NoiseX = FMath::PerlinNoise1D(TimeElapsed * 10.0f) * ShakeIntensityX * DecayFactor;
    float NoiseY = FMath::PerlinNoise1D((TimeElapsed + 100) * 10.0f) * ShakeIntensityY * DecayFactor;
    float NoiseRotZ = FMath::PerlinNoise1D((TimeElapsed + 200) * 10.0f) * ShakeIntensityRotZ * DecayFactor;

    // Apply offset based on Perlin noise
    FVector NewLocation = InitialLocation + FVector(NoiseX, NoiseY, 0);
    FRotator NewRotation = InitialRotation + FRotator(0, 0, NoiseRotZ);
    
    CameraBoom->SetRelativeLocation(NewLocation);
    CameraBoom->SetRelativeRotation(NewRotation);
}


void UCameraShakeComponent::StopShake()
{
    if (!CameraBoom) return;
    // GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("time shook: %f"), TimeElapsed));
    // Reset camera position and rotation
    CameraBoom->SetRelativeLocation(InitialLocation);
    CameraBoom->SetRelativeRotation(InitialRotation);

    GetWorld()->GetTimerManager().ClearTimer(ShakeTimerHandle);
}

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CameraShakeComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUMPSTERDIVERS_API UCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraShakeComponent();

protected:
	virtual void BeginPlay() override;

private:
	// Shake settings
	UPROPERTY(EditAnywhere, Category = "Shake Settings")
	float ShakeDuration = 1.0f;  

	UPROPERTY(EditAnywhere, Category = "Shake Settings")
	float ShakeFrequency = 0.016f; // Update interval = 60 hz

	// Separate intensities for different directions
	UPROPERTY(EditAnywhere, Category = "Shake Intensity")
	float ShakeIntensityX = 5.0f; 
	UPROPERTY(EditAnywhere, Category = "Shake Intensity")
	float ShakeIntensityY = 5.0f; 
	UPROPERTY(EditAnywhere, Category = "Shake Intensity")
	float ShakeIntensityRotZ = 2.0f; 

	// Timer to update shake effect
	FTimerHandle ShakeTimerHandle;
	FTimerHandle StopShakeTimerHandle;
	FVector InitialLocation;
	FRotator InitialRotation;
	float TimeElapsed = 0.0f; // Used for Perlin noise time input

	void ApplyShake();
	void StopShake();
	
	UPROPERTY()
	USceneComponent* CameraBoom;

public:
	void StartShake(float Duration, float IntensityX, float IntensityY, float IntensityRotZ);
	void InitializeCameraShake(USceneComponent* CameraBoomInstance);
};


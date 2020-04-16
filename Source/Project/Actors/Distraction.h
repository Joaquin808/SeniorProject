// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Distraction.generated.h"

UCLASS()
class PROJECT_API ADistraction : public APawn
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere)
	class UPawnNoiseEmitterComponent* NoiseEmitterComp;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float NoiseLevel;

	UPROPERTY(EditAnywhere)
	float NoiseRange;

	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* DistractionAudio;

	FTimerHandle TimerHandle_DistractionTimer;

	FTimerHandle TimerHandle_MakeNoiseTimer;

	class AProjectCharacter* Player;

	// which sound in the Distractions sound cue is for this distraction source
	UPROPERTY(EditDefaultsOnly)
	int32 ParamNumber;

public:	
	// Sets default values for this actor's properties
	ADistraction();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionComp;

	bool bIsDistracting;

	UPROPERTY(EditDefaultsOnly)
	int32 StencilValue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Distract();

	void CreateNoise();

public:	

	void StartDistraction();
	
	void StopDistraction();

	void EnableOutlineEffect();

	void RemoveOutlineEffect();

};

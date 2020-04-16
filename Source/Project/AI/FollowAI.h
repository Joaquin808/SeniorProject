// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FollowAI.generated.h"

UCLASS()
class PROJECT_API AFollowAI : public ACharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* FootstepAudioComp;

	FVector AILocation;

	FVector PlayerLocation;

	bool bSawPlayer;

	bool bLostPlayer;

	FTimerHandle TimerHandle_NotMoving;

	UPROPERTY(EditDefaultsOnly)
	float ChaseWalkSpeed;

	UPROPERTY(EditDefaultsOnly)
	float PatrolWalkSpeed;

	// how close to the player the AI needs to be in order to display it's footsteps
	UPROPERTY(EditDefaultsOnly)
	float ShowFootstepsDistance;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* FeetOutline;

	class AProjectCharacter* PlayerReference;

	UPROPERTY(EditDefaultsOnly)
	int32 StencilValue;

	bool bAIOutlined;

	// how close the AI needs to be to the player in order to stop following them/end game
	UPROPERTY(EditDefaultsOnly)
	float StopFollowDistance;

	float DefaultSightRadius;

	UPROPERTY(EditDefaultsOnly)
	float FollowSightRadius;

	class AProjectCharacter* SeenPlayer;

	FTimerHandle TimerHandle_SeenPlayerTimer;

	UPROPERTY(EditDefaultsOnly)
	float SeenPlayerTimerLength;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> RandomPatrolPoints;

	// the percentage that of when I want the AI to travel to a random point within their regular patrol path
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))
	int32 RandomPatrolPercentage;

	int32 NbrOfPatrolPoints;

	int32 CurrentPatrolIndex;

	FVector CurrentPatrolPointLocation;

	UPROPERTY(EditDefaultsOnly)
	float PatrolRadius;

	FTimerHandle TimerHandle_PatrolAroundPlayer;

	// the min and max amount of time before the next random movement around player occurs
	UPROPERTY(EditDefaultsOnly)
	float PatrolRandMin;

	UPROPERTY(EditDefaultsOnly)
	float PatrolRandMax;

	int32 PatrolAroundPlayerTick;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxPatrolAroundPlayerTicks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* DetectionAudioComp;

	bool bPlayHearNoise;

	FTimerHandle TimerHandle_HearNoiseTimer;

	// how long before the AI should be able to play their OnHearNoise detection audio clip
	UPROPERTY(EditDefaultsOnly)
	float HearNoiseTimerDuration;

	UPROPERTY(EditDefaultsOnly)
	bool bPlayAudio;

	bool bHeardPlayerOnce;

	int32 HeardTimer;

	// how many intervals do I want the AI to hear the player before they move to that location
	UPROPERTY(EditDefaultsOnly)
	int32 MaxHeardIntervalTimer;

	float DefaultHearingThreshold;

	float DefaultsLOSHearingThreshold;

	UPROPERTY(EditDefaultsOnly)
	float HeardHearingThreshold;

	UPROPERTY(EditDefaultsOnly)
	float HeardLOSHearingThreshold;

	UPROPERTY(EditDefaultsOnly)
	float DetectedHearingThreshold;

	UPROPERTY(EditDefaultsOnly)
	float DetectedLOSHearingThreshold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* RandomAudioComp;

	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundCue*> RandomSoundCues;

	// the time in between playing a random sound
	UPROPERTY(EditDefaultsOnly)
	float RandomSoundTimer;

	FTimerHandle TimerHandle_AudioTimer;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* IntroSoundCue;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* VentSoundCue;

	UPROPERTY(EditDefaultsOnly)
	float KillPlayerDistance;

	bool bIsChasingPlayer;

	FTimerHandle TimerHandle_ChasePlayer;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* KillPlayerMontage;

	bool bKilledPlayer;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float KillMontageRate;

	FVector KillLocation;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* KillPlayerAudioComp;

	UPROPERTY(EditDefaultsOnly)
	bool bDebugMessages;

	UPROPERTY(EditDefaultsOnly)
	bool bGameCanEnd;

	bool bIsDistracted;

	FTimerHandle TimerHandle_DistractedTimer;

	UPROPERTY(EditDefaultsOnly)
	float DistractedTimerLength;

	class ADistraction* Distraction;

	TArray<class ADistraction*> PastDistractions;

public:
	// Sets default values for this character's properties
	AFollowAI();

	bool bIsPatroling;

	bool bGoingAfterHeardPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Patrol();

	void StartPatrolTimer(float InRate);

	void PatrolTimerEnd();

	FVector PatrolPoint();

	FVector RandomPatrolPoint();

	void PatrolAroundPlayer();

	void PatrolAroundPlayerTimerEnd();

	void CheckNotMoving();

	void CheckLocation();

	UFUNCTION()
	void OnPawnSeen(APawn* OtherActor);

	void ChasePlayer();

	void CheckDistanceToTarget();

	bool ChaseTimerActive();

	void ClearChaseTimer();

	UFUNCTION()
	void OnHearPawn(APawn* OtherActor, const FVector& Location, float Volume);

	void PlaySeenDetectionNoise();

	void PlayHearDetectionNoise();

	void CanPlayHearNoiseAgain();

	void ShowFootsteps();

	void OutlineFeet(bool bOutlineFeet);

	void ClearSeenPlayerTimer();

	void UpdateHearingRadiusAfterDetection();

	void JumpOutVent();

	void PlayRandomSound();

	void StartAudioTimer(float InRate);

	void OnAudioTimerEnd();

	bool AudioTimerActive();

	void PlayVentAudio();

	void GetReadyToKillPlayer();

	void KillPlayer();

	void ShowGameOverScreen();

	bool GetDistracted();

	void StartDistractionTimer();

	void StopDistractionTimer();

	void LogMessage(FString Message);

public:	

	void OutlineAI(bool bOutlineAI);

	// called when the player has retrieved all of the keys
	void EndGameChasePlayer();

};

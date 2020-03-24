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

	FVector PatrolStartLocation;

	FVector AILocation;

	FVector PlayerLocation;

	UPROPERTY(EditDefaultsOnly)
	float NonDetectedPatrolRadius;

	UPROPERTY(EditDefaultsOnly)
	float DetectedPatrolRadius;

	float PatrolRadius;

	bool bIsPatroling;

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

	UPROPERTY(EditDefaultsOnly)
	bool bDebugMessages;

public:
	// Sets default values for this character's properties
	AFollowAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Patrol();

	void PatrolTimerEnd();

	void CheckNotMoving();

	void CheckLocation();

	void ShowFootsteps();

	void OutlineFeet(bool bOutlineFeet);

	void ClearSeenPlayerTimer();

public:	

	UFUNCTION()
	void OnPawnSeen(APawn* OtherActor);

	UFUNCTION()
	void OnHearPawn(APawn* OtherActor, const FVector& Location, float Volume);

	void OutlineAI(bool bOutlineAI);

};

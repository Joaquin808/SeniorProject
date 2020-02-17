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

	TArray<class AEnvironmentalObjects*> FootstepsArray;

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPawnSeen(APawn* OtherActor);

	UFUNCTION()
	void OnHearPawn(APawn* OtherActor, const FVector& Location, float Volume);

};
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

	UPROPERTY(EditDefaultsOnly)
	float PatrolRadius;

	bool bIsPatroling;

public:
	// Sets default values for this character's properties
	AFollowAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Patrol();

	void PatrolTimerEnd();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnPawnSeen(APawn* OtherActor);

	UFUNCTION()
	void OnHearPawn(APawn* OtherActor, const FVector& Location, float Volume);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "BossAI.generated.h"

UCLASS()
class PROJECT_API ABossAI : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly)
	class ASword* Weapon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASword> WeaponClass;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;

	bool bIsBlocking;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockingMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockingHitMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> AttackAnimations;

	float Health;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	class AProjectCharacter* PlayerReference;

	UPROPERTY(EditDefaultsOnly)
	float DistanceToPlayerThreshold;

	bool bIsAttacking;

	bool bBlockedHit;

	int32 MontageIndex;

	FTimerHandle TimerHandle_EventTimer;

	class ARoamingPoint* RoamingPoint1;

	class ARoamingPoint* RoamingPoint2;

	class ARoamingPoint* RoamingPoint3;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARoamingPoint> RoamingPointClass;

	UPROPERTY(EditAnywhere)
	FVector RoamingPoint1Location;

	UPROPERTY(EditAnywhere)
	FVector RoamingPoint2Location;

	UPROPERTY(EditAnywhere)
	FVector RoamingPoint3Location;

	FTimerHandle TimerHandle_MoveAroundPlayer;

	FVector AIMoveToLocation;

    UPROPERTY(EditDefaultsOnly)
    float StunnedDuration;

    bool bHitWasBlocked;

public:
	// Sets default values for this character's properties
	ABossAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ApproachPlayer();

	void Roll();

	void CombatChoice();

	void CheckDistanceToPlayer();

	void SpawnRoamingPoints();

	void MoveAroundPlayer();

	void StartRoamingTimer();

	void StopRoamingTimer();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Damage(float Damage) override;

	void HitWasBlocked();

    void BlockedHitDone();

};

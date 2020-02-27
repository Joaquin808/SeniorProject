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

	UPROPERTY(EditAnywhere)
	FVector RoamingPoint1RelativeLocation;

	FVector RoamingPoint1Location;

	class ARoamingPoint* RoamingPoint2;

	UPROPERTY(EditAnywhere)
	FVector RoamingPoint2RelativeLocation;

	FVector RoamingPoint2Location;

	class ARoamingPoint* RoamingPoint3;

	UPROPERTY(EditAnywhere)
	FVector RoamingPoint3RelativeLocation;

	FVector RoamingPoint3Location;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARoamingPoint> RoamingPointClass;

public:
	// Sets default values for this character's properties
	ABossAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ApproachPlayer();

	void Attack();

	void Block();

	void UnBlock();

	void Roll();

	void CombatChoice();

	void CheckDistanceToPlayer();

	UAnimMontage* MontageToPlay();

	void ClearTimer();

	void MoveAroundPlayer();

	void SpawnRoamingPoints();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Damage(float Damage) override;

};

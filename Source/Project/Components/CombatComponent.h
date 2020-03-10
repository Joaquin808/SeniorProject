// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework\Character.h"
#include "CombatComponent.generated.h"

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockingMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockingHitMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> AttackAnimations;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	float Health;

	bool bIsAttacking;

	bool bIsBlocking;

	bool bBlockedHit;

	bool bHitWasBlocked;

	int32 MontageIndex;

	FTimerHandle TimerHandle_EventTimer;

	UPROPERTY(EditDefaultsOnly)
	float StunnedDuration;

	ACharacter* Owner;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TakeDamage(float Damage);

	void Attack();

	UAnimMontage* MontageToPlay();

	void Block();

	void UnBlock();

	void Dodge();

	void HitWasBlocked();

	void BlockedHitDone();

	void ClearTimer();
		
};

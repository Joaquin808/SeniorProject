// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "BossAI.generated.h"

UCLASS()
class PROJECT_API ABossAI : public ACharacter, ICombatInterface
{
	GENERATED_BODY()

protected:

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

public:
	// Sets default values for this character's properties
	ABossAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Damage(float Damage) override;

};

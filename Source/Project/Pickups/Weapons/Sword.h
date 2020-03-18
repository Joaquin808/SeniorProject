// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "Sword.generated.h"

class UBoxComponent;

UCLASS()
class PROJECT_API ASword : public APickup
{
	GENERATED_BODY()

protected:

	ASword();

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision1;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision2;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision3;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision4;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision5;

	TArray<UBoxComponent*> CollisionBoxes;

	FTimerHandle TimerHandle_Attack;

	UPROPERTY(EditDefaultsOnly)
	float AttackDistance;

	AActor* Owner;

	UPROPERTY(EditDefaultsOnly)
	float Damage;

	TArray<AActor*> HitActors;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugLines;

public:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SwordMesh;

protected:

	virtual void BeginPlay() override;

public:

	void Attack();

	UFUNCTION(BlueprintCallable)
	void StartAttack();

	UFUNCTION(BlueprintCallable)
	void StopAttack();
	
};

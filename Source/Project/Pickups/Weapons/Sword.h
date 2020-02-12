// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "Sword.generated.h"

UCLASS()
class PROJECT_API ASword : public APickup
{
	GENERATED_BODY()

protected:

	ASword();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SwordMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;
	
};

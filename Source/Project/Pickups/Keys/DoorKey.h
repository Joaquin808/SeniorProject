// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "DoorKey.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_API ADoorKey : public APickup
{
	GENERATED_BODY()

protected:

	ADoorKey();

	USceneComponent* DefaultRoot;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere)
	int32 StencilValue;

public:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* KeyMesh;

public:

	void EnableOutline();

	void DisableOutline();
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentalObjects.generated.h"

UCLASS()
class PROJECT_API AEnvironmentalObjects : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	int32 StencilValue;
	
	UPROPERTY(EditAnywhere)
	bool bAlwaysOutlined;

public:	
	// Sets default values for this actor's properties
	AEnvironmentalObjects();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionComp;

	bool bIsOutlined;

	bool bWasRanInto;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void EnableOutlineEffect();

	void RemoveOutlineEffect();

};

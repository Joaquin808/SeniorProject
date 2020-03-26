// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadBody.generated.h"

UCLASS()
class PROJECT_API ADeadBody : public AActor
{
	GENERATED_BODY()

protected:

	//USceneComponent* DefaultRoot;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMesh;

	class AProjectCharacter* PlayerReference;

	FTimerHandle TimerHandle_DistanceToPlayer;
	
	bool bIsOutlined;

	UPROPERTY(EditDefaultsOnly)
	float DistanceToPlayerThreshold;

	UPROPERTY(EditDefaultsOnly)
	int32 StencilValue;

public:	
	// Sets default values for this actor's properties
	ADeadBody();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CheckDistanceToPlayer();

	void Outline(bool bOutline);

};

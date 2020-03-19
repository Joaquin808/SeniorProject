// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorTravelPoints.generated.h"

UCLASS()
class PROJECT_API ADoorTravelPoints : public AActor
{
	GENERATED_BODY()

protected:

	USceneComponent* DefaultRoot;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Point1Collider;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Point1;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Point2Collider;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Point2;

	FVector Point1Location;

	FVector Point2Location;

	FTimerHandle TimerHandle_OverlapTimer;

	UPROPERTY(EditInstanceOnly)
	class AEnvironmentalObjects* Door;

	UPROPERTY(EditAnywhere)
	float OverlapTimer;
	
public:	
	// Sets default values for this actor's properties
	ADoorTravelPoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OverlapTimerEnd();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap1Begin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlap2Begin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentalObjects.generated.h"

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	Open,
	Closed,
	Locked,
	Unlock
};

UCLASS()
class PROJECT_API AEnvironmentalObjects : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAudioComponent* DoorAudioComp;

	UPROPERTY(EditAnywhere)
	int32 StencilValue;
	
	UPROPERTY(EditAnywhere)
	bool bAlwaysOutlined;

	UPROPERTY(EditAnywhere)
	bool bIsToy;

	FCollisionResponseContainer DefaultCollisionRespones;

public:	
	// Sets default values for this actor's properties
	AEnvironmentalObjects();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionComp;

	bool bIsOutlined;

	bool bWasRanInto;

	UPROPERTY(EditAnywhere)
	bool bIsWall;

	class AProjectCharacter* PlayerReference;

	UPROPERTY(EditAnywhere)
	bool bIsDoor;

	UPROPERTY(BlueprintReadOnly)
	bool bDoorIsOpen;

	UPROPERTY(EditAnywhere)
	bool bIsDoorLocked;

	UPROPERTY(EditAnywhere)
	FString DoorKeyName;

	UPROPERTY(EditAnywhere)
	bool bIsMasterDoor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void EnableOutlineEffect();

	void RemoveOutlineEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void OpenDoor();

	UFUNCTION(BlueprintImplementableEvent)
	void CloseDoor();

	// whenever the door is open the player should be able to walk through
	UFUNCTION(BlueprintCallable)
	void DoorIsOpen();

	// whenever the door is closed, the player shouldn't be allowed to just walk through the door
	UFUNCTION(BlueprintCallable)
	void DoorIsClosed();

	void PlayDoorSound(EDoorType DoorType);

};

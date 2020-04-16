// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "ProjectCharacter.generated.h"

UENUM(BlueprintType)
enum class EStance : uint8
{
	Unarmed,
	Armed,
	Combat
};

class UInputComponent;

UCLASS(config=Game)
class AProjectCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* Collider;

	UPROPERTY(VisibleAnywhere)
	class UPawnNoiseEmitterComponent* NoiseEmitterComp;

	UPROPERTY(VisibleAnywhere)
	class UPointLightComponent* PointLight;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;

	bool bIsCrouched;

	float FootStepVolume;

	float FootStepRange;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WalkingFootStepVolume;

	UPROPERTY(EditDefaultsOnly)
	float WalkingFootStepRange;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CrouchedFootStepVolume;

	UPROPERTY(EditDefaultsOnly)
	float CrouchedFootStepRange;

	UPROPERTY(BlueprintReadOnly)
	class ASword* Weapon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASword> WeaponClass;

	UPROPERTY(BlueprintReadOnly)
	EStance Stance;

	UPROPERTY(EditDefaultsOnly)
	int32 TraceDistance;

	UPROPERTY(EditDefaultsOnly)
	float SphereRadius;

	TArray<class AEnvironmentalObjects*> RenderedObjects;

	FTimerHandle TimerHandle_SonarCooldown;

	UPROPERTY(EditDefaultsOnly)
	float SonarCooldownRate;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAbilityLight> LightClass;

	class AAbilityLight* Light;

	UPROPERTY(EditDefaultsOnly)
	float LightHeight;

	UPROPERTY(EditDefaultsOnly)
	float AbilityLightHeight;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAbilityLight> AbilityLightClass;

	class AAbilityLight* AbilityLight;

	UPROPERTY(EditDefaultsOnly)
	bool bOutlineObjects;

	UPROPERTY(EditDefaultsOnly)
	bool bUseLight;

	UPROPERTY(EditDefaultsOnly)
	bool bUseOverheadLight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUsePointLight;

	UPROPERTY(EditDefaultsOnly)
	float PointLightRate;

	UPROPERTY(EditDefaultsOnly)
	float PointLightRadius;

	UPROPERTY(BlueprintReadWrite)
	bool bAbilityIsActive;

	TMap<FString, class APickup*> Inventory;

	UPROPERTY(EditDefaultsOnly)
	float LineTraceDistance;

	class AEnvironmentalObjects* Door;

	UPROPERTY(BlueprintReadOnly)
	float FPS;

	float DeltaTime;

	FTimerHandle TimerHandle_FPSTimer;

	UPROPERTY(EditDefaultsOnly)
	float KeySphereRadius;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawCheckForPickupsSphere;

	class APickup* FoundPickup;

	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* CombatAudioComp;

	UPROPERTY(VisibleAnywhere)
	class UAudioComponent* DeathAudioComp;

	UPROPERTY(BlueprintReadWrite)
	class UCombatUI* HealthBar;

	UPROPERTY(EditDefaultsOnly)
	bool bGameEndWithCar;

	class AEnvironmentalObjects* Car;

public:

	AProjectCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// I want these public so the AI can use them
	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocking;

    class ABossAI* BossAIReference;

	UPROPERTY(EditDefaultsOnly)
	bool bOutlineWalls;

	UPROPERTY(EditDefaultsOnly)
	bool bDebugMode;

	class AFollowAI* FollowAI;

	bool bIsHiding;

	bool bOverlappingHidingSpot;

	AActor* HidingSpot;

	bool bIsInRoom;

	APlayerController* PlayerController;

	int32 NumberOfKeysInGame;

	TArray<AActor*> ActiveDistractions;

	class ADistraction* Distraction;

	TArray<class ADistraction*> RenderedDistractions;

protected:

	virtual void BeginPlay();
	
	void Attack();

	void Block();

	void UnBlock();

	void Ability();

	void DoCrouch();

	void Dodge();

	void SonarCooldown();

	UFUNCTION(BlueprintImplementableEvent)
	void ExpandLight();

	UFUNCTION(BlueprintImplementableEvent)
	void ReverseLight();

	UFUNCTION(BlueprintCallable)
	void CheckLightDistanceToAI(float LightRadius);

	void CheckForInteractions();

	void Interact();

	void CheckForDoors();

	void InteractWithDoor();

	void CheckForPickups();

	void InteractWithPickups();

	bool PlayerHasKeyForDoor(class AEnvironmentalObjects* Door);

	bool PlayerHasItemInInventory(class APickup* Item);

	void UpdateFPS();

	void InteractWithHidingSpot();

	void Hide();

	void UnHide();

	void OutlineMeshAndSword();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void StartCheckForCarTimer();

	void CheckForCar();

	void InteractWithCar();

	void CheckForDistractions();

	void InteractWithDistractions();

	void LoadWinGameScreen();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void Damage(float Damage) override;

	void HitWasBlocked();

	void BlockedHitDone();

	void DieToFollowAI();

	void PlayDeathAudio();

	bool PlayerHasAllKeys();

	/** Returns Mesh1P subobject **/
	//FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};


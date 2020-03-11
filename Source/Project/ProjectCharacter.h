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

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	//UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	//class USkeletalMeshComponent* Mesh1P;

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

	UPROPERTY(BlueprintReadOnly)
	class ASword* Weapon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASword> WeaponClass;

	UPROPERTY(BlueprintReadOnly)
	EStance Stance;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* AttackMontage;

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
	UAnimMontage* BlockingMontage;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockingHitMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUsePointLight;

	UPROPERTY(EditDefaultsOnly)
	float PointLightRate;

	UPROPERTY(EditDefaultsOnly)
	float PointLightRadius;

    UPROPERTY(EditDefaultsOnly)
    float StunnedDuration;

	UPROPERTY(BlueprintReadWrite)
	bool bAbilityIsActive;

	TMap<FString, class APickup*> Inventory;

	UPROPERTY(EditDefaultsOnly)
	float LineTraceDistance;

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

	float Health;

	bool bIsAttacking;

    class ABossAI* BossAIReference;

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

	void CheckForPickups();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:

	virtual void Damage(float Damage) override;

	void HitWasBlocked();

	void BlockedHitDone();

	/** Returns Mesh1P subobject **/
	//FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};


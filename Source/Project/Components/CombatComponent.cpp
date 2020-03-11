// Fill out your copyright notice in the Description page of Project Settings.
#include "CombatComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCombatComponent::TakeDamage(float Damage)
{
	UE_LOG(LogTemp, Log, TEXT("Damage done to BossAI"));
	if (bIsBlocking)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EventTimer, this, &UCombatComponent::ClearTimer, 0.1f, false, Owner->PlayAnimMontage(BlockingHitMontage));
		bBlockedHit = true;
		return false;
	}
	else
	{
		Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
		return true;
	}
}

void UCombatComponent::Attack()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_EventTimer)) //|| GetWorldTimerManager().IsTimerActive(TimerHandle_MoveAroundPlayer))
	{
		return;
	}

	bIsAttacking = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EventTimer, this, &UCombatComponent::ClearTimer, 0.1f, false, Owner->PlayAnimMontage(MontageToPlay()));
	MontageIndex++;
	if (MontageIndex > (AttackAnimations.Num() - 1))
	{
		MontageIndex = 0;
	}
}

UAnimMontage* UCombatComponent::MontageToPlay()
{
	return AttackAnimations[MontageIndex];
}

void UCombatComponent::Block()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_EventTimer))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Block"));
	bIsBlocking = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EventTimer, this, &UCombatComponent::ClearTimer, 0.1f, false, Owner->PlayAnimMontage(BlockingMontage));
}

void UCombatComponent::UnBlock()
{
	bIsBlocking = false;
	Owner->StopAnimMontage(BlockingMontage);
}

void UCombatComponent::Dodge()
{

}

void UCombatComponent::HitWasBlocked()
{
	if (bHitWasBlocked)
	{
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_EventTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EventTimer);
	}

	Owner->PlayAnimMontage(BlockingHitMontage);

	Owner->GetController()->StopMovement();

	bHitWasBlocked = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EventTimer, this, &UCombatComponent::BlockedHitDone, 0.1f, false, StunnedDuration);
}

void UCombatComponent::BlockedHitDone()
{
	Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bHitWasBlocked = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EventTimer);
}

void UCombatComponent::ClearTimer()
{
	bIsAttacking = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EventTimer);
}

